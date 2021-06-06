#include <syscall.h>
#include <uart.h>
#include <exec.h>
#include <current.h>
#include <fork.h>
#include <interrupt.h>
#include <preempt.h>
#include <syscall_wrapper.h>
#include <file.h>
#include <asm/errno.h>

SYSCALL_DEFINE0(getpid) {
    return current->pid;
}

SYSCALL_METADATA(exec);
SYSCALL_METADATA(exit);
SYSCALL_METADATA(fork);
SYSCALL_METADATA(open);
SYSCALL_METADATA(read);
SYSCALL_METADATA(write);
SYSCALL_METADATA(close);
SYSCALL_METADATA(uart_read);
SYSCALL_METADATA(uart_write);
SYSCALL_METADATA(getcwd);
SYSCALL_METADATA(chdir);
SYSCALL_METADATA(mkdir);
SYSCALL_METADATA(rmdir);
SYSCALL_METADATA(mount);
SYSCALL_METADATA(umount);
SYSCALL_METADATA(sleep);
SYSCALL_METADATA(fsync);

long not_implemented(const struct pt_regs *regs) {
    return -ENOSYS;
}

syscall syscall_table[NR_syscalls] = {
    [0 ... NR_syscalls-1] = &not_implemented,
    SYSCALL(SYS_UART_READ, uart_read),
    SYSCALL(SYS_UART_WRITE, uart_write),
    SYSCALL(SYS_EXEC, exec),
    SYSCALL(SYS_GETPID, getpid),
    SYSCALL(SYS_EXIT, exit),
    SYSCALL(SYS_FORK, fork),
    SYSCALL(SYS_OPEN, open),
    SYSCALL(SYS_READ, read),
    SYSCALL(SYS_WRITE, write),
    SYSCALL(SYS_CLOSE, close),
    SYSCALL(SYS_GETCWD, getcwd),
    SYSCALL(SYS_CHDIR, chdir),
    SYSCALL(SYS_MKDIR, mkdir),
    SYSCALL(SYS_RMDIR, rmdir),
    SYSCALL(SYS_MOUNT, mount),
    SYSCALL(SYS_UMOUNT, umount),
    SYSCALL(SYS_SLEEP, sleep),
    SYSCALL(SYS_FSYNC, fsync)
};