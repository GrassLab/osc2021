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

int sys_getpid();
size_t sys_uart_write(const char buf[], size_t size);
size_t sys_uart_read(char buf[], size_t size);

int sys_exec(const char *name, char *const args[]);
void sys_exit();
int sys_fork(const struct trap_frame *tf);