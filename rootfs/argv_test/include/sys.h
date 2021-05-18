#ifndef	_SYS_H
#define	_SYS_H

#include "types.h"

#define __NR_syscalls	        9

#define SYS_WRITE_NUMBER        0 		// syscal numbers 
#define SYS_UARTWRITE_NUMBER    1
#define SYS_UARTREAD_NUMBER     2
#define SYS_GETPID_NUMBER       3
#define SYS_FORK_NUMBER         4
#define SYS_EXEC_NUMBER         5
#define SYS_EXIT_NUMBER         6
#define SYS_MALLOC_NUMBER       7 	
#define SYS_CLONE_NUMBER        8

#ifndef __ASSEMBLER__

void sys_write(char * buf);
int sys_uart_write(char buf[], size_t size);
int sys_uart_read(char buf[], size_t size);
int sys_gitPID();
int sys_fork();
int sys_exec(const char* name, char* const argv[]);
void sys_exit();
void *sys_malloc(int bytes);
int sys_clone();

void call_sys_write(char * buf);
int call_sys_uart_write(char buf[], size_t size);
int call_sys_uart_read(char buf[], size_t size);
int call_sys_gitPID();
int call_sys_fork();
int call_sys_exec(const char* name, char* const argv[]);
void call_sys_exit();
void *call_sys_malloc();

#endif
#endif  /*_SYS_H */