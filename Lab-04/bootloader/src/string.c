#include "string.h"

void *memset(void *s, int c, unsigned long n){
    char *xs = s;
    while (n--)
        *xs++ = c;
    return s;
}


int strcmp(const char *a, const char *b){
    if(!(*a | *b)) return 0;
    return (*a != *b)? *a - *b : strcmp(++a, ++b);
}


int strlen(const char* s){
    int len = 0;
    while(s[len] != '\0'){
        len++;
    }
    return len;
}

