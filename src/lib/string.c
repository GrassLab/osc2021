#include "string.h"

int strcmp(char *str1, char *str2)
{
    int result = 1;
    int i = 0;

    if (strlen(str1) == strlen(str2)) {
        while(str1[i] != '\0' && str2[i] != '\0')
        {
            result = 0;
            if (str1[i] != str2[i]) {
                return 1;
            }

            i++;
        }
    }

    return result;
}

int strlen(char *str) 
{
    int len = 0;
    while (*str != '\0')
    {
        len++;
        str++;
    }

    return len;
}

void * memset (void *ptr, int value, int num)
{
    char *currentPtr = ptr;

    while (num--) {
        *currentPtr = (unsigned char)value;
        currentPtr++;
    }

    return ptr;
}


int hextoi(char *hexStr)
{
    int value = 0;
    int tmpValue;
    while (*hexStr != '\0') {
        if (*hexStr >= 65 && *hexStr <= 70) {
            tmpValue = *hexStr - 55;
        } else if (*hexStr >= 97 && *hexStr <= 102) {
            tmpValue = *hexStr - 87;
        } else {
            tmpValue = *hexStr - '0';
        }

        value = (value << 4) + tmpValue;
        hexStr++;
    }

    return value;
}