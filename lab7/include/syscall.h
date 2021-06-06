#ifndef __USER_SYSCALL_H_
#define __USER_SYSCALL_H_

#include <sysreg.h>

#define _STR(x) #x
#define STR(s) _STR(s)

typedef long (*syscall)(const struct pt_regs *);

enum {
    SYS_UART_READ,
    SYS_UART_WRITE,
    SYS_EXEC,
    SYS_GETPID,
    SYS_EXIT,
    SYS_FORK,
    SYS_READ,
    SYS_WRITE,
    SYS_OPEN,
    SYS_CLOSE,
    SYS_GETCWD,
    SYS_CHDIR,
    SYS_MKDIR,
    SYS_RMDIR,
    SYS_MOUNT,
    SYS_UMOUNT,
    SYS_SLEEP,
    SYS_FSYNC,
    NR_syscalls
};

extern syscall syscall_table[];

#endif