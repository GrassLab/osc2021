#include "include/mini_uart.h"
#include "utils.h"
#include "include/syscall.h"
#include "include/csched.h"
#include "include/cutils.h"
#include "include/cirq.h"
#include "include/entry.h"
#include "include/mm.h"
#include "include/vfs.h"

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

int sys_open(const char *pathname, int flags)
{
    struct file *file_handle;

    if (!(file_handle = vfs_open(pathname, flags)))
        return -1; // fail
    for (int i = 0; i < 8; ++i) {
        if (!(current->fd_tab[i])) {
            current->fd_tab[i] = file_handle;
            return i;
        }
    }
    return -1; // fail
}

int sys_read(int fd, void *buf, int count)
{
    return vfs_read(current->fd_tab[fd], buf, count);
}

int sys_write(int fd, const void *buf, int count)
{
    return vfs_write(current->fd_tab[fd], buf, count);
}

int sys_close(int fd)
{
    int ret;
    if (current->fd_tab[fd])
        ret = vfs_close(current->fd_tab[fd]);
    current->fd_tab[fd] = 0;

    return ret;
}

int sys_stat_and_next(int fd, struct dentry *dent)
{
    struct file *fh;
    struct vnode *child;

    fh = current->fd_tab[fd];
    if (!(child = fh->child_iter)) {
        fh->vnode->v_ops->get_child(fh->vnode, &(fh->child_iter));
        return 1;
    }
    child->v_ops->stat(child, dent);   // stat
    child->v_ops->get_rsib(child, &(fh->child_iter)); // next

    return 0;
}

int sys_mkdir(const char *path, int mode)
{
    return vfs_mkdir(path, mode);
}

int sys_chdir(const char *path)
{
    return vfs_chdir(path);
}

int sys_mount(const char* device, const char* mountpoint,
    const char* filesystem)
{
    return vfs_mount(device, mountpoint, filesystem);
}

int sys_umount(const char* mountpoint)
{
    return vfs_umount(mountpoint);
}

int sys_sync()
{
    return vfs_sync();
}

int sys_mknod(char *pathname, int devnum)
{
    return vfs_mknod(pathname, devnum);
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
    uart_send_string("From syscall_handler: A\r\n");
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
        case SYS_OPEN:
            ret = sys_open((char*)x0, (int)x1);
            break;
        case SYS_READ:
            ret = sys_read((int)x0, (void*)x1, (int)x2);
            break;
        case SYS_WRITE:
            ret = sys_write((int)x0, (void*)x1, (int)x2);
            break;
        case SYS_CLOSE:
            ret = sys_close((int)x0);
            break;
        case SYS_STAT_AND_NEXT:
            ret = sys_stat_and_next((int)x0, (struct dentry*)x1);
            break;
        case SYS_MKDIR:
            ret = sys_mkdir((char*)x0, (int)x1);
            break;
        case SYS_CHDIR:
            ret = sys_chdir((char*)x0);
            break;
        case SYS_MOUNT:
            ret = sys_mount((char*)x0, (char*)x1, (char*)x2);
            break;
        case SYS_UMOUNT:
            ret = sys_umount((char*)x0);
            break;
        case SYS_SYNC:
            ret = sys_sync();
            break;
        case SYS_MKNOD:
            ret = sys_mknod((char*)x0, (int)x1);
            break;
        default:
            uart_send_string("Error: Unknown syscall type.");
    }
    return ret;
}