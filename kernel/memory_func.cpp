#include <kernel/memory_func.h>

void *memset(void *s, int c, size_t n) {
    for (int i = 0; i < n; i++) {
        ((char*)s)[i] = (char)c;
    }
    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    for (int i = 0; i < n; i++) {
        ((char*)dest)[i] = ((char*)src)[i];
    }
    return dest;
}
