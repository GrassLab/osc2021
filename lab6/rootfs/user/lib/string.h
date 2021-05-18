#ifndef STRING_H
#define STRING_H
#include "types.h"

int strlen(const char *str);
int strncmp(const char *str1, const char *str2, uint32_t n);
char *strncpy(char *dest, const char *src, uint32_t n);
void memset(char* buf, size_t size, char c);
long strtol(const char *__restrict nptr, char **__restrict endptr, int base);
void* memcpy (void *dest, const void *src, size_t len);
int printf (const char *format, ...);
#endif
