#ifndef _LCD_UTIL_H_
#define _LCD_UTIL_H_

#include "type.h"

char* itoa(int num, char* str, int base);
void itohexstr(uint64_t d, int size, char * s);

int atoi(const char *str);

void print_memory_with_uart(void *address, int size);

#endif