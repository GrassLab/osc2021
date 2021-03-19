#ifndef _STRING_H
#define _STRING_H
#include "uart.h"

void *memset(void *s, int c, unsigned long n);
int strcmp(const char *a, const char *b);
int strlen(const char* s);
unsigned long hextoint(char* addr, const int size);





#endif /*_STRING_H */
