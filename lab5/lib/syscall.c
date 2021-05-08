#include <syscall.h>
#include <uart.h>
#include <exec.h>
#include <current.h>

inline void disable_interrupt() {
    write_sysreg(DAIFSet, 0xf);
}

inline void enable_interrupt() {
    write_sysreg(DAIFClr, 0xf);
}

void sys_uart_read(struct pt_regs *regs) {
    char c = _getchar();
    regs->regs[0] = c;
}

void sys_uart_write(struct pt_regs *regs) {
    _putchar(regs->regs[0]);
    regs->regs[0] = 0;
}

void sys_exec(struct pt_regs *regs) {
    /* TODO: should guarantee memory is in userland */
    const char *path = (char *)regs->regs[0];
    const char **args = (const char **)regs->regs[1];

    do_exec(path, args);
}

void sys_getpid(struct pt_regs *regs) {
    regs->regs[0] = (size_t)current->pid;
}

/* TODO: fix this up */
void sys_exit(struct pt_regs *regs) {
    /* ensure we won't get inturrupted here */
    //disable_interrupt();
}

syscall syscall_table[NR_syscalls] = {
    [SYS_UART_READ] = &sys_uart_read,
    [SYS_UART_WRITE] = &sys_uart_write,
    [SYS_EXEC] = &sys_exec,
    [SYS_GETPID] = &sys_getpid,
    [SYS_EXIT] = &sys_exit
};