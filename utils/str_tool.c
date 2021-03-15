// #include "stdint.h"

int strcmp(char *s1, char *s2){
    while(*s1!='\0' && *s2!='\0' && *s1==*s2){
        s1 += 1;
        s2 += 1;
    }
    return *s1 - *s2;
}

// char* itoa(int64_t num, int base){
//     static char buf[30] = {0};
//     int i = 25;
//     if(num == 0){
//         buf[i] = '0';
//         return &buf[i];
//     }

//     for(; num && i; i--, num/=base)
//         buf[i] = "0123456789abcdef"[num%base];
    
//     return &buf[i+1];
// }