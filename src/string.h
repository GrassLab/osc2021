#ifndef STRING_H
#define STRING_H
#include "data_type.h"

void mem_set (char *dst, char value, u64 size);
void strip_newline (char *buffer);
int strcmp (char *str1, char *str2);
int strlen (char *str);
void strncopy (char *d, char *r, unsigned int len);
unsigned long htoui (char *b);
unsigned long atoui (char *b);
long atoi (char *b);

#endif
