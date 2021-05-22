#ifndef __USER_STDLIB_H_
#define __USER_STDLIB_H_
#include <stddef.h>

#define O_RDONLY  00000000
#define O_WRONLY  00000001
#define O_RDWR    00000002
#define O_CREAT   00000100

typedef long ssize_t;

int getpid();
size_t uart_read(const char *buf, size_t size);
size_t uart_write(const char *buf, size_t size);
int exec(const char *path, char *const argv[]);
int fork();
void exit(int status);
int open(const char *pathname, int flags);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int close(int fd);

#endif