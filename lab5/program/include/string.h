#pragma once

#include "types.h"

size_t strlen(const char * str);
char* u64toa(uint64_t number, char *output, size_t count);
char* u64tohex(uint64_t number, char *output, size_t count);
char* strcpy(char* dst, const char* src);
