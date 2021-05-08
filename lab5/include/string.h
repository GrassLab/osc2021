#ifndef __UTILS_H_
#define __UTILS_H_

#include <stddef.h>

int strncmp(const char *a, const char *b, size_t n);
int strcmp(const char *a, const char *b);
size_t strlen(const char *s);
void memcpy(char *dst, const char *src, size_t n);
char *strdup(const char *s);
void *memset(void *s, int c, size_t n);

#endif