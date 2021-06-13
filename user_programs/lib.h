#pragma once

#include "stddef.h"
#include <stdarg.h>

#define FILE_O_CREAT (1 << 0)

// == lib.s
// syscalls for user programs
int getpid();
size_t uart_read(char buf[], size_t size);
size_t uart_write(const char buf[], size_t size);
int exec(const char *name, const char **args);
void exit();
int fork();
int open(const char *pathname, int flags);
int close(int fd);
int write(int fd, const void *buf, int count);
int read(int fd, void *buf, int count);

// == stdio.c
void printf(char *fmt, ...);
