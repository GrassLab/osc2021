#pragma once
#include <stdint.h>
#include <stddef.h>

int strcmp(const char *, const char *);
int strncmp(const char *str1, const char *str2, size_t n);
int strlen(const char *str);

char* itoa(int val, int base);