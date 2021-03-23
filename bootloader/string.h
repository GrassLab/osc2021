#ifndef _LCD_STRING_H_
#define _LCD_STRING_H_

#include "type.h"

int strcmp(char *str1, char *str2);
int strlen(char *str);
void itohexstr(uint64_t d, int size, char * s);

#endif