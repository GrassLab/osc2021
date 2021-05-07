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

char* u64tohex(uint64_t number, char *output, size_t count) {
    if (count < 19) {
        return nullptr;
    }
    output[0] = '0';
    output[1] = 'x';
    for (int i = 15; i >= 0; i--) {
        char ch = (number >> (i << 2)) & 0xf;
        if (ch <= 9) ch += '0';
        else ch += ('a' - 10);
        output[15 - i + 2] = ch;
    }
    output[18] = 0;
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

