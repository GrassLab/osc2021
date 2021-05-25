#pragma once

#include <stdarg.h>
#include <stddef.h>

#define O_CREAT 1

unsigned long uart_read(char buf[], size_t size);
unsigned long uart_write(const char buf[], size_t size);
int getpid();
unsigned long fork();
int exec(const char *name, char *const argv[]);
void exit();
int open(const char *pathname, int flags);
int close(int fd);
int write(int fd, const void *buf, int count);
int read(int fd, void *buf, int count);
int list(int fd, void *buf, int index);
int mkdir(const char *pathname);
int chdir(const char *pathname);
int mount(const char* device, const char* mountpoint, const char*
filesystem); int umount(const char* mountpoint);
