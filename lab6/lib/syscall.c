#include <syscall.h>
#include <uart.h>
#include <exec.h>
#include <current.h>
#include <fork.h>
#include <interrupt.h>
#include <preempt.h>
#include <syscall_wrapper.h>

SYSCALL_DEFINE2(uart_read, char *, buf, int, count) {
    for (int i = 0; i < count; i++) {
        buf[i] = _getchar();
    }

    return count;
}

SYSCALL_DEFINE2(uart_write, char *, buf, int, count) {
    for (int i = 0; i < count; i++) {
        _putchar(buf[i]);
    }

    return count;
}

SYSCALL_DEFINE2(exec, const char *, path, const char **, argv) {
    return do_exec(path, argv);
}

SYSCALL_DEFINE0(getpid) {
    return current->pid;
}

SYSCALL_DEFINE1(exit, int, status) {
    kill_task(current, status);

    return -1;
}

SYSCALL_DEFINE0(fork) {
    return do_fork();
}

syscall syscall_table[NR_syscalls] = {
    SYSCALL(SYS_UART_READ, uart_read),
    SYSCALL(SYS_UART_WRITE, uart_write),
    SYSCALL(SYS_EXEC, exec),
    SYSCALL(SYS_GETPID, getpid),
    SYSCALL(SYS_EXIT, exit),
    SYSCALL(SYS_FORK, fork),
};