#include "utils.h"
#include "mm.h"

/* Delay time = time * 3~4 cycles */
void delay(uint64_t time) {
    while (time--) {
        asm volatile("nop");
    }
}

size_t strlen(const char *s) {
    size_t count = 0;
    while (*(s + count))
        count++;
    return count;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1++ != *s2++)
            return -1;
    }
    return *s1 == *s2 ? 0 : -1;
}

void strcpy(char *s1, const char *s2) {
    while (*s2)
        *(s1++) = *(s2++);
    *s1 = (char)0;
}

void strncpy(char *s1, const char *s2, size_t size) {
    for (size_t i = 0; i < size; i++)
        *(s1++) = *(s2++);
    *s1 = (char)0;
}

void strcat(char *s1, const char *s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    for (size_t i = 0; i < len2; i++) {
        *(s1 + len1 + i) = *(s2 + i);
    }
    *(s1 + len1 + len2) = (char)0;
}

int strchr(const char *s, char c) {
    size_t len = strlen(s);
    for (int i = 0; i < len; i++) {
        if (*(s + i) == c)
            return i;
    }
    return -1;
}

int ctoi(char c) {
    if ((int)c >= 48 && (int)c <= 57) {
        return (int)c - 48;
    } else if ((int)c >= 65 && (int)c <= 70){
        return (int)c - 55;
    } else {
        return 0;
    }
}

unsigned int atoi(const char *s) {
    unsigned int pos = 0;
    unsigned int num = 0;
    while(s[pos++]) {
        num *= 10;
        num += ctoi(s[pos - 1]);
    }
    return num;
}

uint8_t* htoa(uint64_t num) {
    static uint16_t count = 0;
    if (!num) {
        uint8_t *s = kmalloc(16+1);
        return s;
    }
    uint8_t *s = htoa(num / 16);
    uint8_t n = num % 16;
    if (n < 10) {
        s[count++] = '0' + n;
    } else {
        s[count++] = 'A' + (n - 10);
    }
    return s;
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
            size_t size,
            char data) {
    for (size_t i = 0; i < size; i++)
        *((char*)addr + i) = data;
}

void memcpy(void *tar_addr,
            void *src_addr,
            size_t size) {
    for (size_t i = 0; i < size; i++)
        *((char*)tar_addr + i) = *((char*)src_addr + i);
}
