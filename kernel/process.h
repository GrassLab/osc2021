#ifndef PROCESS_H
#define PROCESS_H

#include "../lib/type.h"

#define PROCESS_SIZE 4096

int do_getpid();
size_t do_uart_read(char buffer, size_t size);
size_t do_uart_write(const char buffer[], size_t size);
void do_fork();
void do_exec(const char * name, const char * argv[]);
void do_exit();

int do_open(const char * path_name, int flags);
int do_close(int fd);
int do_write(int fd, const void * buf, size_t len);
int do_read(int fd, void * buf, size_t len);

#endif