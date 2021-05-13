#ifndef _SYSTEM_CALL_H
#define _SYSTEM_CALL_H

#include "def.h"

void sys_schedule();
void sys_uart_read();
void sys_uart_write();
void sys_exec(char *name, char *argv[]);
int sys_getpid();
int sys_fork();
int sys_exit();
int sys_open(const char* pathname, int flags);
int sys_read(int fd, void* buf, size_t len);
void sys_close(int fd);
int sys_write(int fd, void* buf, size_t len);

#endif