#include "sys.h"
#include "context.h"
#include "utils.h"
#include "cpio.h"
#include "mm.h"
#include "set_int.h"
#include "uart.h"
#include "sched.h"

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
    for (int j = 0; j < file_list[pos].file_size; j++)
        *(exec_addr + j) = *(file_list[pos].file_content + j);
    program_num++;
    /* Count argc */
    while (argv[argc++] != 0) {}

    from_el1_to_el0(argc - 1, argv,
                   (uint64_t)current->ustack_addr[current->ustack_num - 1] + USTACK_SIZE,
                   (uint64_t)exec_addr);
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

    /* Copy parent user stack to child user stack */
    uint64_t parent_usp_start = (uint64_t)parent->ustack_addr[parent->ustack_num - 1] + USTACK_SIZE - 1;
    uint64_t child_usp_start  = (uint64_t)child->ustack_addr[child->ustack_num - 1] + USTACK_SIZE - 1;
    uint64_t parent_usp_end   = frame->sp_el0;
    uint64_t child_usp_end    = child_usp_start - (parent_usp_start - parent_usp_end);
    for (int i = 0; i <= (parent_usp_start - parent_usp_end) ; i++)
        *((char*)(child_usp_start - i)) = *((char*)(parent_usp_start - i));
    /* Copy trap frame to child kernel stack*/
    uint64_t parent_ksp_start = (uint64_t)frame;
    uint64_t child_ksp_start  = child->reg.sp;
    for (int i = 0; i < 272; i++)
        *((char*)(child_ksp_start + i)) = *((char*)(parent_ksp_start + i));
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
    }
    frame->x[8] = 0;
}
