#include "string.h"
#include "mini_uart.h"
#include "def.h"

int strcmp(const char *str1, const char *str2)
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

int strcpy(char *dest, const char *src)
{
    int i = 0, length = strlen(src);
    for (; i < length; i++) {
        dest[i] = src[i];
    }

    dest[i] = '\0';

    return 0;
}

char *strncpy(char *dest, const char *src, size_t count)
{
    int i = 0;
    for (; i < count; i++) {
        dest[i] = src[i];
    }

    dest[i] = '\0';

    return 0;
}



int strlen(const char *str) 
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

char *itoa(unsigned int value, char *str, int base)
{   
    int i = 0;
    if (value == 0) {
        str[0] = '0';
    } else {
        while(value != 0)
        {
            int mod = value % base; 
            if (base == 16 && mod > 9) {
                str[i] = mod - 10 +'A';
            } else {
                str[i] = mod + '0';
            }
            value = value / base;

            i++;
        }
    }
    
    int len = strlen(str);

    for (int low = 0, high = len - 1; low < high; low++, high--)
    {
        int temp = str[low];
        str[low] = str[high];
        str[high] = temp;
    }

    return str;
}

char *litoa(unsigned long int value, char *str, int base)
{   
    int i = 0;
    if (value == 0) {
        str[0] = '0';
    } else {
        while(value != 0)
        {
            int mod = value % base; 
            if (base == 16 && mod > 9) {
                str[i] = mod - 10 +'A';
            } else {
                str[i] = mod + '0';
            }
            value = value / base;

            i++;
        }
    }
    
    int len = strlen(str);

    for (int low = 0, high = len - 1; low < high; low++, high--)
    {
        int temp = str[low];
        str[low] = str[high];
        str[high] = temp;
    }

    return str;
}