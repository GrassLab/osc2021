#ifndef __SYS_CALL_H
#define __SYS_CALL_H

#include <stdint.h>
#include <stddef.h>

extern void delay(int cycles);

int getpid();
size_t uart_read(char buf[], size_t size);
size_t uart_write(const char buf[], size_t size);
int exec(const char* name, char *const argv[]);
void exit();
int fork();

#endif
