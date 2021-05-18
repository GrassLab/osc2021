#include "utils.h"

/* Delay time = time * 3~4 cycles */
void delay(uint64_t time) {
    while (time--) {
        asm volatile("nop");
    }
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1++ != *s2++)
            return -1;
    }
    return *s1 == *s2 ? 0 : -1;
}

int char_2_int(char c) {
    if ((int)c >= 48 && (int)c <= 57) {
        return (int)c - 48;
    } else if ((int)c >= 65 && (int)c <= 70){
        return (int)c - 55;
    } else {
        return 0;
    }
}

unsigned int str_2_int(const char *s) {
    unsigned int pos = 0;
    unsigned int num = 0;
    while(s[pos++]) {
        num *= 10;
        num += char_2_int(s[pos - 1]);
    }
    return num;
}

unsigned long pow(unsigned int base,
                  unsigned int order) {
    unsigned long sum = 1;
    for (int i = 0; i < order; i++) {
        sum *= base;
    }
    return sum;
}

void memset(void *addr,
            unsigned long size,
            char data) {
    for (unsigned long i = 0; i < size; i++) {
        *((char*)addr + i) = data;
    }
}
