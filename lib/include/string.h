#ifndef STRING_H
#define STRING_H
#include "args.h"

void memset (char *dst, char n, unsigned int size);
void mem_set (char *dst, char value, unsigned long size);
void strip_newline (char *buffer);
int strcmp (const char *str1, const char *str2);
int strlen (const char *str);
void strncopy (char *d, const char *r, unsigned int len);
void memcpy (unsigned char *d, unsigned char *s, unsigned int size);
long strfind (const char *src, char token);
unsigned long htoui (char *b);
unsigned long atoui (char *b);
long atoi (char *b);
int snprintf (char *dst, unsigned int size, char *format, ...);
int vsnprintf (char *dst, unsigned int size, char *format, va_list args);
#endif
