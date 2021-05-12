#pragma once

#include <types.h>

int strcmp(const char* str1, const char* str2);
size_t strcpy_size(char* dst, const char* src);
char* u64tohex(uint64_t number, char *output, size_t count);
