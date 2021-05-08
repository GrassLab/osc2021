#pragma once

#include "types.h"

extern "C" {
    uint64_t strcmp(const char* str1, const char* str2);
}
size_t strcpy_size(char* dst, const char* src);
