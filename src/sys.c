#include "sys.h"
#include "printf.h"
#include "mm.h"
#include "cpio.h"
#include "sched.h"
#include "uart.h"
#include "fork.h"
#include "string.h"

void sys_write(char * buf) 
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
    for (int i = 0;i < size;i++) {
        buf[i] = uart_getc();
    }
    buf[size] = '\0';

    return size;
}

int sys_gitPID()
{
    return current->pid;
}

int sys_fork()
{
    preempt_disable();

    unsigned long user_stack = (unsigned long) kmalloc(PAGE_SIZE);
    int pid = copy_process(0, 0, 0, user_stack);
    //printf("[sys_fork] New child process pid = %d\n", pid);

    // full copy user stack
    memcpy(task[pid]->stack, current->stack, PAGE_SIZE);

    // Set proper user stack sp to new user process
    // New user stack sp should have same offset as parent process sp
    struct pt_regs * cur_regs = task_pt_regs(current);
    int copiedTask_sp_offset = cur_regs->sp - current->stack;
    struct pt_regs *childregs = task_pt_regs(task[pid]);
    childregs->sp = task[pid]->stack + copiedTask_sp_offset;
    preempt_enable();
    

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
    unsigned long move_address = 0x10A0000;
    for (i = 0;i < strlen((char *)name);i ++) {
        filename_buf[i] = name[i];
    }
    for (int temp = i;i < strlen(extension) + temp;i++) {
        filename_buf[i] = extension[i - temp];
    }
    void *target_addr = cpio_move_file((void *) INITRAMFS_ADDR, filename_buf, move_address);
    //void *target_addr = cpio_get_file((void *) INITRAMFS_ADDR, "fork_test.img", &unused); // why cause error?
    
    // count argv[] until terminated by a null pointer
    int argc_count = 0;
    while (argv[argc_count] != 0) {
        argc_count++;
    }
    
    // Reset user sp and move argv to user stack(for argument passing)
    // Note that sp must 16 byte alignment
    struct pt_regs *regs = task_pt_regs(current);
    char **backup = kmalloc(PAGE_SIZE);
    for (int i = 0;i < argc_count;i++) {
        *(backup + i) = argv[i];
    }
    regs->sp = current->stack + PAGE_SIZE;
    regs->sp = regs->sp - ((argc_count + argc_count % 2) * 8);
    char **temp = (char **)regs->sp;
    for (int i = 0;i < argc_count;i++) {
        *(temp + i)  = *(backup + i);
    }

    // set pc(elr_el) to new function(user program), and starting address of argv[]
    regs->pc = (unsigned long)target_addr;
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
    //   Not required in lab
    return 0;
}

void * const sys_call_table[] = 
    {sys_write, sys_uart_write, sys_uart_read, 
     sys_gitPID, sys_fork, sys_exec, 
     sys_exit, sys_malloc, sys_clone};