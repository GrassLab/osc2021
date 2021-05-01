#include "utils.h"



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

unsigned long hextoint(char* addr, const int size){
    unsigned long res = 0;
    char c;
    for(int i = 0 ;i < size; ++i){
        res <<= 4;
        c = *(addr + i);
        if(c >= '0' && c <= '9') res += c - '0';
        else if(c >= 'A' && c <= 'F') res += c - 'A' + 10;
        else if(c >= 'a' && c <= 'f') res += c - 'a' + 10;
    }
    return res;
}


void swap(void **a, void **b) {
  void *tmp = *a;
  *a = *b;
  *b = tmp;
}

