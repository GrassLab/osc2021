#ifndef __SYS_CALL_H
#define __SYS_CALL_H

#include <stdint.h>
#include <stddef.h>

#define O_CREAT 0b1

extern void delay(int cycles);

int getpid();
size_t uart_read(char buf[], size_t size);
size_t uart_write(const char buf[], size_t size);
int exec(const char* name, char *const argv[]);
void exit();
int fork();
int open(const char *pathname, int flags);
int close(int fd);
int write(int fd, const void *buf, int count);
int read(int fd, void *buf, int count);

#endif
