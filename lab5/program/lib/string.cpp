#include "../include/string.h"

size_t strlen(const char * str) {
    int offset = 0;
    while (str[offset] != 0) {
        offset++;
    }
    return offset;
}

char* u64toa(uint64_t number, char *output, size_t count) {
    char buffer[25];
    int offset = 0;
    if (count < 2) {
        return nullptr;
    }
    while (number) {
        buffer[offset] = '0' + (number % 10);
        number = number / 10;
        offset++;
        if (count < offset + 1) {
            return nullptr;
        }
    }
    if (offset == 0) {
        strcpy(output, "0");
        return output;
    }
    for (int i = offset; i > 0; i--) {
        output[offset - i] = buffer[i - 1];
    }
    output[offset] = 0;
    return output;
}

char* strcpy(char* dst, const char* src) {
    uint64_t count = 0;
    while (src[count]) {
        dst[count] = src[count];
        count++;
    }
    dst[count] = src[count];
    count++;
    return dst;
}

