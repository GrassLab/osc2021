#ifndef __USER_STDLIB_H_
#define __USER_STDLIB_H_
#include <stddef.h>

int getpid();
size_t read(const char *buf, size_t size);
size_t write(const char *buf, size_t size);
int exec(const char *path, char *const argv[]);

#endif