#include "mystring.h"

char *itoa(int value, char *s, int base) {
    int idx = 0;
    if(value<0) {
        value *= -1;
        s[idx++] = '-';
    }

    char tmp[10];
    int j = 0;
    do {
        int t_val = value % base;
        if(t_val >= 10) {
            tmp[j++] = 'a' + t_val - 10;
        }
        else {
            tmp[j++] = '0' + t_val;
        }
        value /= base;
    } while(value != 0 && j<=10);

    for(int i=j-1;i>=0;i--) {
        s[idx++] = tmp[i];
    }
    s[idx] = '\0';

    return s;
}

long atoi(char *s) {
    long i = 0;
    while(*s) {
        i = i * 10 + *s - '0';
        s++;
    }
    return i;
}

int strcmp(char* a, char* b) {
    int flag = 0;
    while(flag==0) {
        if(*a > *b) {
            return 1;
        }
        else if(*a < *b) {
            return -1;
        }

        if(*a==0) {
            break;
        }
        a++;
        b++;
    }
    return flag;
}

int strncmp(char* a, char* b, unsigned long n) {
    for(int i=0;i<n;i++) {
        if(a[i] != b[i]) {
            return a[i] - b[i];
        }
        if(a[i] == 0) {
            return 0;
        }
    }
    return 0;
}

char *memcpy(char *dest, char *src, unsigned long n) {
    char *d = dest;
    char *s = src;
    while(n--) {
        *d++ = *s++;
    }
    return dest;
}

void memset(char* a, unsigned int value, unsigned int size) {
    for(unsigned int i=0;i<size;i++) {
        a[i] = value;
    }
}

unsigned int vsprintf(char *dest, char *fmt, __builtin_va_list args) {
    char *dest_orig = dest;

    while(*fmt) {
        if(*fmt == '%') {
            fmt++;
            if(*fmt == '%') {
                *dest++ = *fmt;
            }
            else if(*fmt == 'd') {
                int arg = __builtin_va_arg(args, int);
                char buf[12];
                char *p = itoa(arg, buf, 10);
                while(*p) {
                    *dest++ = *p++;
                }
            }
            else if(*fmt == 'x') {
                int arg = __builtin_va_arg(args, int);
                char buf[12];
                char *p = itoa(arg, buf, 16);
                while(*p) {
                    *dest++ = *p++;
                }
            }
        }
        else {
            *dest++ = *fmt;
        }
        fmt++;
    }
    *dest = '\0';

    return dest - dest_orig;
}
