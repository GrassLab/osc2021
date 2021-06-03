#include "string.h"

#include "printf.h"

unsigned int strlen(const char *s){
    int cnt = 0;
    while(s[cnt] != '\0')
        cnt ++;
    return cnt;
}

int strcmp(const char *s1, const char *s2){
    for(;*s1 && *s1 == *s2; s1++, s2++);
    return *s1 - *s2;
}

int strncmp(char *str1, char *str2, int num) {
    for(int i = 0 ; i < num ; i++) {
        char a = str1[i];
        char b = str2[i];
        a = a == 32 ? 0 : a;
        b = b == 32 ? 0 : b;
        if (a == 0 && b == 0) break;
        a = str1[i] >= 'A' && str1[i] <= 'Z' ? str1[i] : str1[i] - 'a' + 'A';
        b = str2[i] >= 'A' && str2[i] <= 'Z' ? str2[i] : str2[i] - 'a' + 'A';
        if(a != b) {
            return 0;
        }
    }
    return 1;
}

int strcmp_eq(char *str1, char *str2) {
    while (1) {
        if ((str1[0] == '\0' && str2[0] == '\0') || (str1[0] == '\0' && str2[0] == 32) || (str1[0] == 32 && str2[0] == '\0')) {
            return 1;
        }
        if (*str1 != *str2) {
            return 0;
        }
        str1++;
        str2++;
    }
}

char *strcpy(char *dest, const char *src){
    char *odest = dest;
    while( (*dest++ = *src++) );
    return odest;
}

char *strncpy(char *dest, const char *src, unsigned long n){
    char *odest = dest;
    while( n-- && (*dest++ = *src++) );
    return odest;
}

char *strrchr(const char *s, char c){
    const char *pos = s, *c_ptr = 0;//NULL
    while(*pos != '\0'){
        if (*pos == c)
            c_ptr = pos;
        pos++;
    }
    return (char *)c_ptr;
}

char *strtok(char *s, const char delim){
    static char *pos;
    char* ret;
    if(s) pos = s;
    
    if(*pos == '\0') return 0;
    // skip leading
    while(*pos == delim){
        pos ++;
    }

    ret = pos;
    while(*pos != delim && *pos != '\0'){
        pos ++;
    }
    if (*pos != '\0'){
        *pos = '\0';
        pos ++;
    }
    return ret;
}

void *memset(void *s, int c, unsigned long n){
    char *xs = s;
    while (n--)
        *xs++ = c;
    return s;
}


