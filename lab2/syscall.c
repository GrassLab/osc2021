#include "include/mini_uart.h"
#include "utils.h"
#include "include/syscall.h"
#include "include/csched.h"
#include "include/cutils.h"
#include "include/entry.h"
#include "include/mm.h"

extern struct task *current;


struct trap_frame* get_trap_frame(struct task *tsk)
{
    return tsk->tf;
    // return (struct trap_frame*)(tsk->kernel_stack_page + 0x1000 - sizeof(struct trap_frame));
}

void set_trap_frame(unsigned long *tf)
{
    current->tf = (struct trap_frame*)tf;
}


unsigned long sys_getpid()
{
    return (unsigned long)current->pid;
}

int sys_uart_read(char buf[], int size)
{
    int cnt = 0;

    for (int i = 0; i < size; ++i) {
        buf[i] = uart_recv();
        cnt++;
    }

    return cnt;
}

int sys_uart_write(const char buf[], int size)
{
    int cnt = 0;

    for (int i = 0; i < size; i++) {
        uart_send(buf[i]);
        cnt++;
    }

    return cnt;
}

int sys_fork(void)
{
    struct task *new;
    struct trap_frame *child_tf;

    if (!(new = new_ts()))
        return 0; // null
    /* Setting task struct */
    // memcpy(new, current, sizeof(struct task));
    new->kernel_stack_page = kmalloc(0x1000); // 4kb
    child_tf = (struct trap_frame*)(new->kernel_stack_page + 0x1000 - sizeof(struct trap_frame));
    new->ksp = (char*)child_tf;
    memcpy((char*)child_tf, (char*)current->tf, sizeof(struct trap_frame));
    new->user_stack_page = kmalloc(0x1000); // 4kb
    memcpy(new->user_stack_page, current->user_stack_page, 0x1000);
    // child has to return to its own stack, and the offset of sp in stack page in same with parent.
    child_tf->sp_el0 = (unsigned long)new->user_stack_page + (current->tf->sp_el0 - (unsigned long)current->user_stack_page);
    new->usp = current->usp;
    new->ctx.x19 = 0; // indicating that this is user fork 
    new->ctx.x30 = (unsigned long)ret_from_fork; // pc
    new->ctx.sp = (unsigned long)new->ksp; // TODO: user space stack pointer.
    new->status = TASK_NEW;
    new->flag = current->flag;
    new->priority = current->priority;
    new->counter = new->priority;
    new->preemptable = 0;
    child_tf->regs[0] = 0;

    add_to_ready(new);
    return new->pid;
}

int sys_exec(unsigned long func_addr, char *const argv[])
{
    int argc, len;
    char *user_sp;
    char **argv_fake, **argv_fake_start;
    struct trap_frame *cur_trap_frame;
delay(10000000);
    /* Get argc, not include null terminate */
    argc = 0;
    while (argv[argc])
        argc++;
    // argv_fake = kmalloc(argc*sizeof(char*));
    argv_fake = (char**)kmalloc(0x1000);
    user_sp = current->user_stack_page + 0x1000;
    for (int i = argc - 1; i >= 0; --i) {
        len = strlen(argv[i]) + 1; // including '\0'
        user_sp -= len;
        argv_fake[i] = user_sp;
        memcpy(user_sp, argv[i], len);
    }
    user_sp -= sizeof(char*); // NULL pointer
    user_sp = align_down(user_sp, 0x8); // or pi will fail
    *((char**)user_sp) = (char*)0;
    for (int i = argc - 1; i >= 0; --i) {
        user_sp -= sizeof(char*);
        *((char**)user_sp) = argv_fake[i];
    }
    // TODO: argv_fake_start: this is
    // temporary. cause now  I don't
    // have solution to conquer the
    // pushing stack issue when
    // starting of function call.
    argv_fake_start = (char**)user_sp;
    user_sp -= sizeof(char**); // char** argv
    *((char**)user_sp) = user_sp + sizeof(char**);
    user_sp -= sizeof(int); // argc
    *((int*)user_sp) = argc;
    kfree((char*)argv_fake);
    current->usp = align_down(user_sp, 0x10);

    cur_trap_frame = get_trap_frame(current);
    cur_trap_frame->regs[0] = (unsigned long)argc;
    cur_trap_frame->regs[1] = (unsigned long)argv_fake_start;
    cur_trap_frame->sp_el0 = (unsigned long)current->usp;
    cur_trap_frame->elr_el1 = func_addr;
    cur_trap_frame->spsr_el1 = 0x0; // enable_irq

    // set_user_program((char*)func_addr, current->usp, argc,
    //     argv_fake_start, current->kernel_stack_page + 0x1000);
    // never come back again.
    return argc;  // Geniusly
}

// DDI0487C_a_armv8_arm.pdf p.2438
unsigned long get_syscall_type()
{
    return get_x8();
}

unsigned long syscall_handler(unsigned long x0, unsigned long x1,
    unsigned long x2, unsigned long x3,
    unsigned long x4, unsigned long x5,
    unsigned long x6, unsigned long x7)
{
    unsigned long type, ret;

    type = get_syscall_type();
    switch(type) {
        case SYS_GET_PID:
            ret = sys_getpid();
            break;
        case SYS_EXEC:
            ret = sys_exec(x0, (char**)x1);
            break;
        case SYS_FORK:
            ret = sys_fork();
            break;
        case SYS_EXIT:
            ret = sys_exit();
            break;
        case SYS_UART_READ:
            ret = sys_uart_read((char*)x0, (int)x1);
            break;
        case SYS_UART_WRITE:
            ret = sys_uart_write((char*)x0, (int)x1);
            break;
        default:
            uart_send_string("Error: Unknown syscall type.");
    }
    return ret;
}