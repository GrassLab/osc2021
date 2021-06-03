#include "stddef.h"

char *itoa(int value, char *s);
unsigned int vsprintf(char *dst, char *fmt, __builtin_va_list args);
unsigned int sprintf(char *dst, char *fmt, ...);
int strcmp(const char *X, const char *Y);
int strncmp(const char *X, const char *Y, int num);
size_t strlen(const char *s);
