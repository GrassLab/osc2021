#ifndef __USER_SYSCALL_H_
#define __USER_SYSCALL_H_

#define _STR(x) #x
#define STR(s) _STR(s)

#define SYS_UART_READ 0
#define SYS_UART_WRITE 1
#define SYS_EXEC 2
#define SYS_GETPID 3
#define SYS_EXIT 4
#define SYS_FORK 5

#endif