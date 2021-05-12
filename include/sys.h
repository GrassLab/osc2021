#ifndef	_SYS_H
#define	_SYS_H

#include "types.h"

#define __NR_syscalls	        16

#define SYS_PRINT_NUMBER        0 		// syscal numbers 
#define SYS_UARTWRITE_NUMBER    1
#define SYS_UARTREAD_NUMBER     2
#define SYS_GETPID_NUMBER       3
#define SYS_FORK_NUMBER         4
#define SYS_EXEC_NUMBER         5
#define SYS_EXIT_NUMBER         6
#define SYS_MALLOC_NUMBER       7 	
#define SYS_CLONE_NUMBER        8
#define SYS_CORETIMER_On        9
#define SYS_CORETIMER_OFF       10
#define SYS_OPEN                11
#define SYS_CLOSE               12
#define SYS_WRITE               13
#define SYS_READ                14
#define SYS_READ_DIRECTORY      15

#ifndef __ASSEMBLER__

#include "vfs.h"
#define SYS_OPEN_FILE_ERROR -1
#define SYS_WRITE_FILE_ERROR -1
#define SYS_READ_FILE_ERROR -1

void sys_print(char * buf);
int sys_uart_write(char buf[], size_t size);
int sys_uart_read(char buf[], size_t size);
int sys_gitPID();
int sys_fork();
int sys_exec(const char* name, char* const argv[]);
void sys_exit();
void *sys_malloc(int bytes);
int sys_clone();
void sys_coreTimer_on();
void sys_coreTimer_off();
int sys_open(const char *pathname, int flags);
int sys_close(int fd);
int sys_write(int fd, const void *buf, size_t len);
int sys_read(int fd, void *buf, size_t len);
char *sys_read_directory(int fd);

void call_sys_print(char * buf);
int call_sys_uart_write(char buf[], size_t size);
int call_sys_uart_read(char buf[], size_t size);
int call_sys_gitPID();
int call_sys_fork();
int call_sys_exec(const char* name, char* const argv[]);
void call_sys_exit();
void* call_sys_malloc();
void call_sys_coreTimer_on();
void call_sys_coreTimer_off();
int call_sys_open(const char *pathname, int flags);
int call_sys_close(int fd);
int call_sys_write(int fd, const void *buf, size_t len);
int call_sys_read(int fd, void *buf, size_t len);
char *call_sys_read_directory(int fd);

#endif /*  __ASSEMBLER__ */
#endif /*_SYS_H */