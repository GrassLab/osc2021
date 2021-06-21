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

int memcmp(const void *s1, const void *s2, size_t n) {
    char *c1 = (char*) s1, *c2 = (char*) s2;
    for (int i = 0; i < n; i++) {
        int val = *c1 - *c2;
        if (val != 0) return val;
    }
    return 0;
}
