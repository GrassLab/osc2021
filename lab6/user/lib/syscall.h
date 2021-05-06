#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include <types.h>
#define O_CREAT 1

extern int getpid();
extern size_t uart_read(char buf[], size_t size);
extern size_t uart_write(const char buf[], size_t size);
extern int exec(const char* name, char *const argv[]);
extern void exit();
extern int fork();
extern size_t get_time();

extern int open(const char *pathname, int flags);
extern int close(int fd);;
extern int write(int fd, const void *buf, int count);
extern int read(int fd, void *buf, int count);
#endif