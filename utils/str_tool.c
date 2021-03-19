#include "stdint.h"

int strcmp(char *s1, char *s2){
    while(*s1!='\0' && *s2!='\0' && *s1==*s2){
        s1 += 1;
        s2 += 1;
    }
    return *s1 - *s2;
}

char* itoa(int64_t val, int base){
    static char buf[32] = {0};
    int i = 30;
    if (val == 0) {
        buf[i] = '0';
        return &buf[i];
    }

    for (; val && i; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];

    return &buf[i + 1];
}