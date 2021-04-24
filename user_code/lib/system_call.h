#pragma once
#define size_t int

int getpid();
size_t uart_read(char buf[], size_t size);
size_t uart_write(const char buf[], size_t size);
int exec(const char *name, char *const argv[]);
void exit();
int fork();