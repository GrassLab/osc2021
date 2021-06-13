#pragma once
#include <stddef.h>
#include <stdint.h>

int strcmp(const char *, const char *);
int strncmp(const char *str1, const char *str2, size_t n);
int strlen(const char *str);
char *strcpy(char *dst, const char *src);
void memcpy(char *dst, const char *src, size_t n);
const char *strchr(const char *s, const char c);

char *itoa(int64_t val, int base);

// Return the fitst char in `s` not being `c`
const char *ignore_leading(const char *s, const char c);

void test_string();