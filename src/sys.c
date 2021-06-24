#include "sys.h"
#include "context.h"
#include "utils.h"
#include "cpio.h"
#include "mm.h"
#include "set_int.h"
#include "uart.h"
#include "sched.h"
#include "vfs.h"
#include "mmu.h"

#define USTACK_SIZE   PAGE_SIZE
#define EXEC_ADDR     0x80000

/* System call - exit */
static void sys_exit(void) {
    thread_t *current = get_current();
    kfree(current->ucode_addr);
    kfree(current->ustack_addr);
    kfree(current->pgd_addr);
    kfree(current->pud_addr);
    kfree(current->pmd_addr);
    kfree(current->pte_addr);
    current->ucode_addr = NULL;
    current->ucode_size = 0;
    current->ustack_addr = NULL;
    current->pgd_addr = NULL;
    current->pud_addr = NULL;
    current->pmd_addr = NULL;
    current->pte_addr = NULL;
    current->reg.ttbr0_el1 = 0;
    thread_exit();
}

void page_fault_handler(uint64_t virtual_addr) {
    print("Segmentation fault: 0x");
    print_hex(virtual_addr);
    print("\n");
    sys_exit();
}

static void page_table_init(thread_t *t) {
    t->pgd_addr = kmalloc(PAGE_SIZE);
    t->pud_addr = kmalloc(PAGE_SIZE);
    t->pmd_addr = kmalloc(PAGE_SIZE);
    t->pte_addr = kmalloc(PAGE_SIZE);

    t->reg.ttbr0_el1 = (uint64_t)t->pgd_addr & PA_MASK;
    uint64_t *entry = t->pgd_addr;
    (*entry) = ((uint64_t)t->pud_addr & PA_MASK) | PGD_ATTR;
    entry = t->pud_addr;
    (*entry) = ((uint64_t)t->pmd_addr & PA_MASK) | PUD_ATTR;
    entry = t->pmd_addr;
    (*entry) = ((uint64_t)t->pte_addr & PA_MASK) | PMD_ATTR;
}

static void pte_init(thread_t *t) {
    uint64_t ustack_addr = EXEC_ADDR - PAGE_SIZE;
    uint32_t page_count = !(t->ucode_size % PAGE_SIZE) ? (t->ucode_size / PAGE_SIZE)
                                                       : (t->ucode_size / PAGE_SIZE) + 1;
    uint64_t *entry;

    entry = (char*)t->pte_addr + (ustack_addr / PAGE_SIZE) * 8;
    (*entry) =  ((uint64_t)t->ustack_addr & PA_MASK) | PTE_NORMAL_ATTR | PD_EL0_ACCESS;
    for(int i = 0; i < page_count; i++) {
        entry = (char*)t->pte_addr + (EXEC_ADDR / PAGE_SIZE + i) * 8;
        (*entry) = (((uint64_t)t->ucode_addr + i * PAGE_SIZE) & PA_MASK) | PTE_NORMAL_ATTR | PD_EL0_ACCESS;
    }
}

/* System call - exec */
int do_exec(const char *name, char *const argv[], int pos) {
    if (pos < 0) {
        if ((pos = search_file(name)) < 0)
            return -1;
    }

    int argc = 0;
    thread_t *current = get_current();

    /* Allocate user stack */
    char *ustack_addr = kmalloc(USTACK_SIZE);
    /* Count argc and Copy argv */
    while (argv[argc] != 0)
        argc++;
    char **tmp_argv = ustack_addr;
    char *kaddr = ustack_addr + argc * sizeof(char*);
    char *uaddr = (EXEC_ADDR - PAGE_SIZE) + argc * sizeof(char*);
    for (int i = 0; i < argc; i++) {
        size_t len = strlen(argv[i]) + 1;
        memcpy(kaddr, argv[i], len);
        tmp_argv[i] = uaddr ;
        kaddr += len;
        uaddr += len;
    }
    if (current->ustack_addr)
        kfree(current->ustack_addr);
    current->ustack_addr = ustack_addr;
    /* load user program */
    if (current->ucode_addr)
        kfree(current->ucode_addr);
    current->ucode_size = file_list[pos].file_size;
    uint32_t alloc_size = !(current->ucode_size % PAGE_SIZE) ? current->ucode_size
                                                             : (current->ucode_size / PAGE_SIZE + 1) * PAGE_SIZE;
    current->ucode_addr = kmalloc(alloc_size);
    memcpy(current->ucode_addr, file_list[pos].file_content, file_list[pos].file_size);
    if(current->pgd_addr == NULL)
        page_table_init(current);
    pte_init(current);

    from_el1_to_el0(argc, EXEC_ADDR - PAGE_SIZE, EXEC_ADDR, current->reg.ttbr0_el1);
}

/*System call - umount */
static void sys_umount(tframe_t *frame) {
    thread_t *current = get_current();
    frame->x[0] = vfs_dir_operation((char*)frame->x[0], &current->wd, 3);
}

/*System call - mount */
static void sys_mount(tframe_t *frame) {
    thread_t *current = get_current();
    frame->x[0] = vfs_mount((char*)frame->x[0], (char*)frame->x[1], (char*)frame->x[2], &current->wd);
}

/*System call - chdir */
static void sys_chdir(tframe_t *frame) {
    thread_t *current = get_current();
    frame->x[0] = vfs_dir_operation((char*)frame->x[0], &current->wd, 2);
}

/* System call - mkdir */
static void  sys_mkdir(tframe_t *frame) {
    thread_t *current = get_current();
    frame->x[0] = vfs_dir_operation((char*)frame->x[0], &current->wd, 1);
}

/* System call - ls */
static void sys_ls(tframe_t *frame) {
    thread_t *current = get_current();
    frame->x[0] = vfs_dir_operation((char*)frame->x[0], &current->wd, 0);
}

/* System call - read */
static void  sys_read(tframe_t *frame) {
    thread_t *current = get_current();
    uint16_t fd = frame->x[0];
    if (fd >= MAX_FD_NUM) {
        frame->x[0] = -1;
        return ;
    }
    frame->x[0] = vfs_read(current->fd_table[fd], frame->x[1], frame->x[2]);
}

/* System call - write */
static void  sys_write(tframe_t *frame) {
    thread_t *current = get_current();
    uint16_t fd = frame->x[0];
    if (fd >= MAX_FD_NUM) {
        frame->x[0] = -1;
        return ;
    }
    frame->x[0] = vfs_write(current->fd_table[fd], frame->x[1], frame->x[2]);
}

/* System call - close */
static void sys_close(tframe_t *frame) {
    thread_t *current = get_current();
    uint16_t fd = frame->x[0];
    if (fd >= MAX_FD_NUM) {
        frame->x[0] = -1;
        return ;
    }
    if (!(frame->x[0] = vfs_close(current->fd_table[fd]))) {
        current->fd_table[fd] = NULL;
        if (current->fd_pos > fd)
            current->fd_pos = fd;
    }
}

/* System call - open */
static void sys_open(tframe_t *frame) {
    thread_t *current = get_current();
    if (current->fd_pos == MAX_FD_NUM) {
        frame->x[0] = -2;
        return ;
    }
    if ((current->fd_table[current->fd_pos] =
         vfs_open((char*)frame->x[0], frame->x[1], current->wd)) != NULL) {
           frame->x[0] = current->fd_pos;
           for (int i = 0; i < MAX_FD_NUM; i++) {
              if (current->fd_table[i] == NULL){
                  current->fd_pos = i;
                  return ;
              }
           }
           current->fd_pos = MAX_FD_NUM;
    } else {
        frame->x[0] = -1;
    }
}

/* System call - sleep */
static void sys_sleep(tframe_t *frame) {
    if (!frame->x[0])
        return ;
    thread_t *current = get_current();
    current->state = wait;
    current->wait_time = frame->x[0];
    schedule();
}

/* System call - fork */
static void sys_fork(tframe_t *frame) {
    thread_t *parent = get_current();
    thread_t *child;
    if ((child = thread_create(&leave_exception)) == NULL) {
        frame->x[0] = -1;
        return ;
    }

    frame->x[0] = child->pid;
    child->reg.sp -= 272;
    child->ustack_addr = kmalloc(USTACK_SIZE);
    child->ucode_size = parent->ucode_size;
    uint32_t alloc_size = !(parent->ucode_size % PAGE_SIZE) ? parent->ucode_size
                                                            : (parent->ucode_size / PAGE_SIZE + 1) * PAGE_SIZE;
    child->ucode_addr = kmalloc(alloc_size);
    memcpy(child->ucode_addr, parent->ucode_addr, parent->ucode_size);
    memcpy(child->ustack_addr, parent->ustack_addr, USTACK_SIZE);
    page_table_init(child);
    pte_init(child);

    child->wd = parent->wd;
    child->fd_pos = child->fd_pos;
    for (int i = 0; i < MAX_FD_NUM; i++) {
        if (parent->fd_table[i] != NULL) {
            child->fd_table[i] = parent->fd_table[i];
            child->fd_table[i]->use_num++;
        }
    }

    /* Copy trap frame to child kernel stack*/
    memcpy((char*)child->reg.sp, (char*)frame, 272);
    /* Modify trap frame */
    tframe_t *child_frame = (tframe_t*)child->reg.sp;
    child_frame->x[0] = 0;
}

/* System call - uart_write */
static void sys_uart_write(tframe_t *frame) {
    frame->x[0] = do_uart_write((char*)frame->x[0], frame->x[1]);
}

size_t do_uart_write(const char *s, size_t size) {
    return async_write(s, size);
}

/* System call - uart_read */
static void sys_uart_read(tframe_t *frame) {
    frame->x[0] = do_uart_read((char*)frame->x[0], frame->x[1]);
}

size_t do_uart_read(char *s, size_t size) {
    return async_read(s, size);
}

/* System call - getpid */
static void sys_getpid(tframe_t *frame) {
    frame->x[0] = do_getpid();
}

int do_getpid(void) {
    thread_t *current = get_current();
    return current->pid;
}

void el0_svc_handler(tframe_t *frame) {
    uint64_t syscall_no = frame->x[8];
    if (syscall_no == SYS_GETPID) {
        sys_getpid(frame);
    } else if (syscall_no == SYS_UART_READ) {
        sys_uart_read(frame);
    } else if (syscall_no == SYS_UART_WRITE) {
        sys_uart_write(frame);
    } else if (syscall_no == SYS_FORK) {
        sys_fork(frame);
    } else if (syscall_no == SYS_EXIT) {
        sys_exit();
    } else if (syscall_no == SYS_SLEEP) {
        sys_sleep(frame);
    } else if (syscall_no == SYS_OPEN) {
        sys_open(frame);
    } else if (syscall_no == SYS_CLOSE) {
        sys_close(frame);
    } else if (syscall_no == SYS_WRITE) {
        sys_write(frame);
    } else if (syscall_no == SYS_READ) {
        sys_read(frame);
    } else if (syscall_no == SYS_LS) {
        sys_ls(frame);
    } else if (syscall_no == SYS_MKDIR) {
        sys_mkdir(frame);
    } else if (syscall_no == SYS_CHDIR) {
        sys_chdir(frame);
    } else if (syscall_no == SYS_MOUNT) {
        sys_mount(frame);
    } else if (syscall_no == SYS_UMOUNT) {
        sys_umount(frame);
    }
    frame->x[8] = 0;
}
