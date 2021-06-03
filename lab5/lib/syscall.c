#include <syscall.h>
#include <uart.h>
#include <exec.h>
#include <current.h>
#include <fork.h>
#include <interrupt.h>
#include <preempt.h>

void sys_uart_read(struct pt_regs *regs) {
    char *buf = (char *)regs->regs[0];
    int count = regs->regs[1];
    for (int i = 0; i < count; i++) {
        buf[i] = _getchar();
    }
    regs->regs[0] = count;
}

void sys_uart_write(struct pt_regs *regs) {
    char *buf = (char *)regs->regs[0];
    int count = regs->regs[1];
    for (int i = 0; i < count; i++) {
        _putchar(buf[i]);
    }
    regs->regs[0] = count;
}

void sys_exec(struct pt_regs *regs) {
    /* TODO: should guarantee memory is in userland */
    const char *path = (char *)regs->regs[0];
    const char **args = (const char **)regs->regs[1];

    regs->regs[0] = do_exec(path, args);
}

void sys_getpid(struct pt_regs *regs) {
    regs->regs[0] = (size_t)current->pid;
}

void sys_exit(struct pt_regs *regs) {
    kill_task(current, regs->regs[0]);
}

void sys_fork(struct pt_regs *regs) {
    regs->regs[0] = do_fork(regs);
}

syscall syscall_table[NR_syscalls] = {
    [SYS_UART_READ] = &sys_uart_read,
    [SYS_UART_WRITE] = &sys_uart_write,
    [SYS_EXEC] = &sys_exec,
    [SYS_GETPID] = &sys_getpid,
    [SYS_EXIT] = &sys_exit,
    [SYS_FORK] = &sys_fork
};