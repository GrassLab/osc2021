#include <stdint.h>
extern "C"
void *memset(uint8_t*dst0, int c0, size_t length) {
    uint8_t c = c0;
    for (size_t i = 0; i < length; i++) {
        dst0[i] = c;
    }
    return dst0;
}
extern "C"
void *memcpy(uint8_t *str1, const uint8_t *str2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        str1[i] = str2[i];
    }
    return str1;
}
