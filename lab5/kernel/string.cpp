#include "string.h"

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
