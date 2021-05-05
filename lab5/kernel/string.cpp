#include "string.h"

uint64_t strcmp(const char* str1, const char* str2) {
    while (true) {
        if (*str1 == 0) {
            return *str2;
        }
        else if (*str2 == 0) {
            return *str1;
        }
        else if (*str1 != *str2) {
            return *str1 - *str2;
        }
        str1++;
        str2++;
    }
}

size_t strcpy_size(char* dst, const char* src) {
    uint64_t count = 0;
    while (src[count]) {
        dst[count] = src[count];
        count++;
    }
    dst[count] = src[count];
    count++;
    return count;
}
