#include <kernel/string.h>
#include <kernel/mini_uart.h>

size_t strcpy_size(char* dst, const char* src) {
    uint64_t count = 0;
    while (src[count]) {
        dst[count] = src[count];
        count++;
    }
    dst[count] = 0;
    count++;
    return count;
}

int strcmp(const char *p1, const char *p2)
{
    const unsigned char *s1 = (const unsigned char *) p1;
    const unsigned char *s2 = (const unsigned char *) p2;
    unsigned char c1, c2;
    do {
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c2 == '\0')
              return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
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
