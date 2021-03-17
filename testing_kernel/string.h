#ifndef _LCD_STRING_H_
#define _LCD_STRING_H_

#include "type.h"

int strcmp(char *str1, char *str2);
int strncmp(const char *a, const char *b, unsigned long n);
int strlen(char *str);
char* strcpy(char *to, const char *from);
void itohexstr(uint64_t d, int size, char * s);

#endif