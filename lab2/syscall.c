#include "include/mini_uart.h"
#include "utils.h"
#include "include/syscall.h"
#include "include/csched.h"
#include "include/cutils.h"
#include "include/entry.h"
#include "include/mm.h"

extern struct task *current;
extern struct task task_pool[];


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

struct wait_h *uartQueue = 0;

void init_uartQueue()
{
    uartQueue = new_wait();
}

#define UART_BUF_SIZE 3
extern char uart_recv_buf[];
extern int uart_recv_buf_in;
extern int uart_recv_buf_out;

int uart_read_block(char buf[], int size)
{
    int tmp, left;

    left = size;
    while (left > 0){
        while ((tmp = uart_read_nonblock(buf+size-left, left)) == EAGAIN) {
            rm_from_ready(current);
            add_to_waitQueue(current, uartQueue);
            schedule();
        }
        left -= tmp;
    }
    return size;
}

int uart_read_nonblock(char buf[], int size)
{
    int read_cnt;
    char ch;
    char *buf_ptr;

    if (uart_recv_buf_in == uart_recv_buf_out)
        return EAGAIN;

    read_cnt = 0;
    buf_ptr = buf;
    while (uart_recv_buf_in != uart_recv_buf_out) {
        ch = uart_recv_buf[uart_recv_buf_out];
        uart_recv_buf_out = (uart_recv_buf_out + 1) % UART_BUF_SIZE;
        *buf_ptr++ = ch;
        read_cnt++;
        if (read_cnt >= size)
            break;
    }
    return read_cnt;
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
    new->sig.sigpend = 0;
    
    add_to_ready(new);
    return new->pid;
}



int sys_sigreturn(unsigned long __unused)
{
    uart_send_string("From sys_sigreturn\r\n");
    current->sig.sigpend = 0;
    kfree(current->sig.sigaltstack);
    back_to_entry();
    return 0 ; // Never return
}

int sys_kill(int pid, int signal)
{
    struct task *target;

    target = &task_pool[pid];
    if (target->free)
        return -1;
    target->sig.sigpend |= (1 << signal);
    return 0;
}

int sys_signal(int signal, unsigned long handler)
{
    current->sig.user_handler[signal] = handler;
    return 0;
}

int sys_useless()
{
    enable_irq();
    int cnt = 0;
    while (cnt < 10) {
        delay(10000000);
        uart_send_int(cnt++);
        uart_send_string("\r\n");
    }
    disable_irq();
    return 0;
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
            // ret = sys_uart_read((char*)x0, (int)x1);
            ret = uart_read_block((char*)x0, (int)x1);
            break;
        case SYS_UART_WRITE:
            ret = sys_uart_write((char*)x0, (int)x1);
            break;
        case SYS_SIGRETURN:
            ret = sys_sigreturn(0);
            break;
        case SYS_KILL:
            ret = sys_kill((int)x0, (int)x1);
            break;
        case SYS_SIGNAL:
            ret = sys_signal((int)x0, x1);
            break;
        case SYS_USELESS:
            ret = sys_useless();
            break;
        default:
            uart_send_string("Error: Unknown syscall type.");
    }
    return ret;
}