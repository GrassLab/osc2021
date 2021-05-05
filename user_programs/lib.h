#pragma once

#include "stddef.h"
#include <stdarg.h>

// == lib.s
// syscalls for user programs
int getpid();
size_t uart_read(char buf[], size_t size);
size_t uart_write(const char buf[], size_t size);
int exec(const char *name, const char **args);
void exit();
int fork();

// == stdio.c
void printf(char *fmt, ...);
