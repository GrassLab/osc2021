#pragma once

#include <types.h>

extern "C" {
    void *memset(void *s, int c, size_t n);
     void *memcpy(void *dest, const void *src, size_t n);
}
