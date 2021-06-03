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

#endif