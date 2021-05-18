#include "string.h"

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
void strcpy(char* dst, const char* content){
    int i = 0;
    do{
        dst[i] = content[i];
    }while(content[i++] != '\0');
}