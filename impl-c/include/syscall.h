#pragma once
#include "exception.h"
#include "stddef.h"
void syscall_routing(int num, struct trap_frame *);

// define syscall numbers
#define SYS_GETPID 1
#define SYS_UART_READ 2
#define SYS_UART_WRITE 3
#define SYS_EXEC 4
#define SYS_EXIT 5
#define SYS_FORK 6
#define SYS_OPEN 7
#define SYS_CLOSE 8
#define SYS_WRITE 9
#define SYS_READ 10

int sys_getpid();
size_t sys_uart_write(const char buf[], size_t size);
size_t sys_uart_read(char buf[], size_t size);

int sys_exec(const char *name, char *const args[]);
void sys_exit();
int sys_fork(const struct trap_frame *tf);

#define SYS_OPEN_MX_FD_REACHED -2
#define SYS_OPEN_FILE_NOT_FOUND -3
int sys_open(const char *pathname, int flags);

int sys_close(int fd);
int sys_write(int fd, const void *buf, int count);
int sys_read(int fd, void *buf, int count);
