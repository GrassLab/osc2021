#ifndef _LCD_STRING_H_
#define _LCD_STRING_H_

#include "type.h"

int strcmp(const char *str1, const char *str2);
int strncmp(const char *a, const char *b, unsigned long n);
int strlen(const char *str);
char* strcpy(char *to, const char *from);
char* reverse(char *buffer, int i, int j);

#endif