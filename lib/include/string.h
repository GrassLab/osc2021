#ifndef _STRING_H
#define _STRING_H

#include "def.h"

int strcmp(const char *str1, const char *str2);
int strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t count);
int strlen(const char *str);
void * memset ( void * ptr, int value, int num );
int hextoi(char *hexStr);
char *itoa(unsigned int value, char *str, int base);
char *litoa(unsigned long int value, char *str, int base);


#endif