#pragma once

#include "stddef.h"

int getpid();
// size_t uart_read(char buf[], size_t size);
size_t uart_write(const char buf[], size_t size);
// int exec(const char *name, char *const argv[]);
// void exit();
