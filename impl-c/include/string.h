#pragma once
#include <stddef.h>
#include <stdint.h>

int strcmp(const char *, const char *);
int strncmp(const char *str1, const char *str2, size_t n);
int strlen(const char *str);
char *strcpy(char *dst, const char *src);

char *itoa(int64_t val, int base);