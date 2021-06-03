#ifndef	_SYS_H
#define	_SYS_H

void call_sys_write(char * buf);
int call_sys_uart_write(char buf[], unsigned int size);
int call_sys_uart_read(char buf[], unsigned int size);
int call_sys_gitPID();
int call_sys_fork();
int call_sys_exec(const char* name, char* const argv[]);
void call_sys_exit();
void *call_sys_malloc();

#endif