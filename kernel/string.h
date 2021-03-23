#ifndef _LCD_STRING_H_
#define _LCD_STRING_H_

#include "type.h"

int strcmp(const char *str1, const char *str2);
int strncmp(const char *a, const char *b, unsigned long n);
int strlen(const char *str);
char* strcpy(char *to, const char *from);
void itoa(int x, char str[], int d);
void itohexstr(uint64_t d, int size, char * s);

void reverse(char *s);


#endif