#include "../include/stringUtils.h"
int compString(char *command, char *buffer){
    unsigned char c1, c2;

    do {
        c1 = (unsigned char)*buffer++;
        c2 = (unsigned char)*command++;
        if (c1 == '\0') {
            return c1 - c2;
        }
    } while (c1 == c2);

    return c1 - c2;


}

unsigned int getIntegerFromString(const char *str) {
    unsigned int value = 0u;

    while (*str) {
        if(*str >= '0' && *str<= '9'){
            value = value * 10u + (*str - '0');
        }
        ++str;
    }
    return value;
}

unsigned long getHexFromString(const char *str) {
    unsigned long value = 0u;

    while (*str) {
        if(*str >= '0' && *str <= '9'){
            value = value*16 + *str - '0';
        }else if(*str >= 'a' && *str <= 'z'){
            value = value*16 + *str - 'a' + 10u;
        }else if(*str >= 'A' && *str <= 'Z'){
            value = value*16 + *str - 'A' + 10u;
        }
        ++str;
    }
    return value;
}
