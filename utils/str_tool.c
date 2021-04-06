#include "stdint.h"

int strcmp(char *s1, char *s2){
    while(*s1!='\0' && *s2!='\0' && *s1==*s2){
        s1 += 1;
        s2 += 1;
    }
    return *s1 - *s2;
}

int32_t strlen(char *s){
    int32_t s_size = 0;
    while(*(s+s_size) != 0)
        s_size += 1;
    return s_size;
}

int64_t atoi(char* buf, int base){
    int64_t val = 0;
    uint16_t buf_len = strlen(buf), i=0, sign=1;
    if(buf[0] == '-'){
        sign = 0;
        i += 1;
    }
    for(; i<buf_len; i++){
        val *= base;
        val += hex_to_int8(buf[i]);
    }
    if(!sign)
        val *= -1;
    
    return val;
}

char* itoa(int64_t val, int base){
    static char buf[32] = {0};
    uint8_t sign = 1;
    if(val < 0){
        sign = 0;
        val = -1*val;
    }
=======
char* itoa(int64_t val, int base){
    static char buf[32] = {0};
    int i = 30;
    if (val == 0) {
        buf[i] = '0';
        return &buf[i];
    }

    for (; val && i; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];

    if(!sign){
        buf[i] = '-';
        i -= 1;
    }

=======
    return &buf[i + 1];
}