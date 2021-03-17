#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

int strcmp(const char *p1, const char *p2);
size_t strlen(const char *str);
int strcmp_n(const char *s1, const char *s2, size_t n);
void *memcpy(void *dest, const void *src, size_t len);
char *strcpy(char *destination, const char *source);
char *strcpy_n(char *destination, const char *source, size_t len);

long long atoi(char *s);
unsigned long long atol(char *s);
long long atoi_n(char *s, size_t max_len, int base);
unsigned long long atol_n(char *s, size_t max_len, int base);

#endif