#include "sys.h"
#include "printf.h"
#include "mm.h"
#include "cpio.h"
#include "sched.h"
#include "uart.h"
#include "fork.h"
#include "string.h"
#include "timer.h"
#include "vfs.h"

void sys_print(char * buf) 
{
    printf(buf);
}

int sys_uart_write(char buf[], size_t size) 
{
    for (int i = 0;i < size;i++) {
        uart_send(buf[i]);
    }

    return size;
}

int sys_uart_read(char buf[], size_t size) 
{
    preempt_disable();

    for (int i = 0;i < size;i++) {
        // block until uart_read ready
        block_uart_read();
        buf[i] = uart_getc();
    }
    buf[size] = '\0';

    preempt_enable();

    return size;
}

int sys_gitPID()
{
    return current->pid;
}

int sys_fork()
{
    preempt_disable();
    #ifdef __DEBUG_MM
    printf("[sys_fork]\n");
    #endif

    int pid = copy_process_virt(0, 0, 0);
    
    preempt_enable();
    
    #ifdef __DEBUG_MM
    printf("[sys_fork] New child process pid = %d\n", pid);
    #endif

    return pid;
}

int sys_exec(const char *name, char* const argv[])
{
    preempt_disable();
    
    // concatenate filename and extension
    // Read cpio file and move target file in cpio to proper starting memory address
    // corresponding to definition of it's starting memory address in linker sciprt 
    char filename_buf[30];
    char extension[] = ".img";
    int i;
    unsigned long move_address = 0x0; // program code will start at 0 (virutal address) 
    for (i = 0;i < strlen((char *)name);i ++) {
        filename_buf[i] = name[i];
    }
    for (int temp = i;i < strlen(extension) + temp;i++) {
        filename_buf[i] = extension[i - temp];
    }
    filename_buf[i] = '\0';

    cpio_move_file(INITRAMFS_ADDR, filename_buf, move_address);
    //void *target_addr = cpio_get_file((void *) INITRAMFS_ADDR, "fork_test.img", &unused); // why cause error?
    
    // count argv[] until terminated by a null pointer
    int argc_count = 0;
    while (argv[argc_count] != 0) {
        argc_count++;
    }
    
    // Reset user sp and move argv to user stack(for argument passing)
    // Note that sp must 16 byte alignment
    struct pt_regs *regs = task_pt_regs(current);
    char **backup = alloacte_kernel_page();
    for (int i = 0;i < argc_count;i++) {
        *(backup + i) = argv[i];
    }
    // sp increase from high memory to low memory
    // And insert all argv
    regs->sp = MAX_PROCESS_ADDRESS_SPACE;
    regs->sp = regs->sp - ((argc_count + argc_count % 2) * 8);
    char **temp = (char **)regs->sp;
    for (int i = 0;i < argc_count;i++) {
        *(temp + i)  = *(backup + i);
    }
    free_page(backup);

    // set pc(elr_el) to new function(user program), and starting address of argv[]
    regs->pc = 0; // Becuase virtual memory enable, so user code can always start at 0
    regs->regs[1] = (unsigned long)regs->sp;
    preempt_enable();

    return argc_count;
}

void sys_exit()
{
    exit_process();
}

void *sys_malloc(int bytes)
{
    // Just call kmalloc for easy
    return kmalloc(bytes);
}

int sys_clone()
{
    // TODO:
    // Not required in lab 5
    return 0;
}

void sys_coreTimer_on()
{
    core_timer_enable();
    printf("[Core timer] interrupt enabled\n");
}

void sys_coreTimer_off()
{
    core_timer_disable();
    printf("[Core timer] interrupt disabled\n");
}

int sys_open(const char *pathname, int flags)
{
    preempt_disable();

    struct file *fd = vfs_open(pathname, flags);

    if (fd == NULL) {
        return SYS_OPEN_FILE_ERROR;
    }
    
    // fill in file descriptor table(fdt) for current task
    struct task_struct *current_task = current; // get current task
    
    if (current_task->files.count >= NR_OPEN_DEFAULT) {
        printf("[sys_open] File Desciprtor Table is full. Open file fail\n");
        return SYS_OPEN_FILE_ERROR;
    }
    
    // fill in fdt and update fdt info
    int current_fd_idx = current_task->files.next_fd;
    current_task->files.fd_array[current_fd_idx] = fd;
    current_task->files.count++;
    for (int i = (current_fd_idx+1) % NR_OPEN_DEFAULT;i != current_fd_idx;i = (i+1) % NR_OPEN_DEFAULT) {
        // circularly search free entry in ftd
        if (current_task->files.fd_array[i] == NULL){
            current_task->files.next_fd = i;
            break;
        }
    }
    //printf("[sys_open] current_fd_idx = %d\n", current_fd_idx);
    //printf("[sys_open] next_fd = %d\n", current_task->files.next_fd);

    preempt_enable();

    return current_fd_idx;
    
}

int sys_close(int fd)
{
    preempt_disable();

    struct task_struct *current_task = current; // get current task
    struct file *file = current_task->files.fd_array[fd]; // get file
    
    // close file and clear fd entry in ftb 
    int res = vfs_close(file);
    current_task->files.fd_array[fd] = NULL;
    current_task->files.count--;

    // _vfs_dump_file_struct();

    preempt_enable();

    return res;
}

int sys_write(int fd, const void *buf, size_t len)
{
    preempt_disable();

    struct task_struct *current_task = current; // get current task
    struct file *file = current_task->files.fd_array[fd]; // get file
    
    if (file == NULL) {
        printf("[sys_write] File Descriptor not exist.");
        return -1;
    }

    // close file and clear fd entry in ftb 
    int nr_byte_written = vfs_write(file, buf, len);

    preempt_enable();
    
    return nr_byte_written;
    
}

int sys_read(int fd, void *buf, size_t len)
{
    preempt_disable();

    struct task_struct *current_task = current; // get current task
    struct file *file = current_task->files.fd_array[fd]; // get file
    
    if (file == NULL) {
        printf("[sys_read] File Descriptor not exist.");
        return -1;
    }

    // close file and clear fd entry in ftb 
    int nr_byte_read = vfs_read(file, buf, len);

    preempt_enable();
    
    return nr_byte_read;
}

char *sys_read_directory(int fd)
{
    // TODO:
    struct task_struct *current_task = current; // get current task
    struct file *file = current_task->files.fd_array[fd]; // get file
    
    if (file == NULL) {
        printf("[sys_read_directory] File Descriptor not exist.");
        return NULL;
    }

    char *buf_ptr = vfs_read_directory(file);

    return buf_ptr;
}

int sys_mkdir(const char *pathname)
{
    return vfs_mkdir(pathname);
}

int sys_chdir(const char *pathname)
{
    return vfs_chdir(pathname);
}

int sys_mount(const char* device, const char* mountpoint, const char* filesystem)
{
    return vfs_mount(device, mountpoint, filesystem);
}

int sys_unmount(const char* mountpoint)
{
    return vfs_unmount(mountpoint);
}

void *sys_mmap(void *addr, size_t len, int prot, int flags, int fd, int file_offset)
{
    return mem_map(addr, len, prot, flags, fd, file_offset);
}   

void * const sys_call_table[] = 
    {sys_print, sys_uart_write, sys_uart_read, 
     sys_gitPID, sys_fork, sys_exec, 
     sys_exit, sys_malloc, sys_clone,
     sys_coreTimer_on, sys_coreTimer_off,
     sys_open, sys_close, sys_write, 
     sys_read, sys_read_directory, sys_mkdir, 
     sys_chdir, sys_mount, sys_unmount,
     sys_mmap};