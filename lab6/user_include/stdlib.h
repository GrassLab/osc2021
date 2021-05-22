#ifndef __USER_STDLIB_H_
#define __USER_STDLIB_H_
#include <stddef.h>

int getpid();
size_t uart_read(const char *buf, size_t size);
size_t uart_write(const char *buf, size_t size);
int exec(const char *path, char *const argv[]);
int fork();
void exit(int status);

#endif