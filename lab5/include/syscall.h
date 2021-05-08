#ifndef __USER_SYSCALL_H_
#define __USER_SYSCALL_H_

#include <sysreg.h>

#define _STR(x) #x
#define STR(s) _STR(s)

typedef void (*syscall)(struct pt_regs *);

enum {
    SYS_UART_READ,
    SYS_UART_WRITE,
    SYS_EXEC,
    SYS_GETPID,
    SYS_EXIT,
    NR_syscalls
};

extern syscall syscall_table[];

#endif