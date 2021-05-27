#include "sys.h"
#include "context.h"
#include "utils.h"
#include "cpio.h"
#include "mm.h"
#include "set_int.h"
#include "uart.h"
#include "sched.h"
#include "vfs.h"

#define USTACK_SIZE       2048
#define EXEC_ADDR         0x1000000
#define MAX_PROGRAM_SIZE  0x1000

uint32_t program_num = 1;

/* System call - exec */
int do_exec(const char *name, char *const argv[], int pos) {
    if (pos < 0) {
        if ((pos = search_file(name)) < 0)
            return -1;
    }

    int argc = 0;
    thread_t *current = get_current();
    if (current->ustack_num > MAX_USTACK_NUM)
        return -1;

    /* Allocate user program address */
    current->ustack_addr[current->ustack_num] = kmalloc(USTACK_SIZE);
    current->ustack_num++;
    /* load user program */
    char *exec_addr = (char*)(EXEC_ADDR + MAX_PROGRAM_SIZE * (program_num - 1));
    memcpy(exec_addr, file_list[pos].file_content, file_list[pos].file_size);
    program_num++;
    /* Count argc */
    while (argv[argc++] != 0) {}

    from_el1_to_el0(argc - 1, argv,
                   (uint64_t)current->ustack_addr[current->ustack_num - 1] + USTACK_SIZE,
                   (uint64_t)exec_addr);
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

/* System call - exit */
static void sys_exit(void) {
    thread_t *current = get_current();
    for (int i = 0; i < current->ustack_num; i++)
        kfree(current->ustack_addr[i]);
    thread_exit();
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
    child->ustack_addr[child->ustack_num] = kmalloc(USTACK_SIZE);
    child->ustack_num++;
    child->wd = parent->wd;
    child->fd_pos = child->fd_pos;
    for (int i = 0; i < MAX_FD_NUM; i++) {
        if (parent->fd_table[i] != NULL) {
            child->fd_table[i] = parent->fd_table[i];
            child->fd_table[i]->use_num++;
        }
    }

    /* Copy parent user stack to child user stack */
    uint64_t parent_usp_start = (uint64_t)parent->ustack_addr[parent->ustack_num - 1] + USTACK_SIZE - 1;
    uint64_t child_usp_start  = (uint64_t)child->ustack_addr[child->ustack_num - 1] + USTACK_SIZE - 1;
    uint64_t parent_usp_end   = frame->sp_el0;
    uint64_t child_usp_end    = child_usp_start - (parent_usp_start - parent_usp_end);
    memcpy((char*)child_usp_end, (char*)parent_usp_end, (parent_usp_start - parent_usp_end + 1));
    /* Copy trap frame to child kernel stack*/
    uint64_t parent_ksp_start = (uint64_t)frame;
    uint64_t child_ksp_start  = child->reg.sp;
    memcpy((char*)child_ksp_start, (char*)parent_ksp_start, 272);
    /* Modify trap frame */
    tframe_t *child_frame = (tframe_t*)child_ksp_start;
    child_frame->x[0] = 0;
    child_frame->sp_el0 = child_usp_end;
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
        sys_umount()
    }
    frame->x[8] = 0;
}
