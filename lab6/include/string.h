#ifndef __UTILS_H_
#define __UTILS_H_

#include <stddef.h>

int strncmp(const char *a, const char *b, size_t n);
int strcmp(const char *a, const char *b);
size_t strlen(const char *s);
char *strsep(char **stringp, const char *delim);
char *strchr(const char *s, int c);
void memcpy(void *dst, const void *src, size_t n);
char *strdup(const char *s);
void *memset(void *s, int c, size_t n);

#endif