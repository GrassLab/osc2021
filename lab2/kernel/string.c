#include "string.h"
#include "uart.h"

int strlcmp(char *s1, char *s2, int len)
{
    if(len == 0)
        return 0;
    
    for(int i = 0; i < len; i++)
    {
        if(s1[i] != s2[i] )
            return 0;
    }
    return 1;
}

int strcmp(char *s1, char *s2)
{
    while(*s1++ == *s2++)
    {
        if(*(s1-1) == '\0' || *(s2-1) == '\0')
        {
            break;
        }
    }
    
    if(*(s1-1) == '\0' && *(s2-1) == '\0')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int str2int(char *str, int len)
{
    int num = 0;
    for(int i = 0; i < len ; i++)
    {
        num *= 16;
        num += *str - '0';
        if((*str - '0') > 9)
            num -= 7;
        str++;
    }
    return num;
}

int strlen(char *str)
{
    int len = 0;
    while(*str++ != '\0')
        len++;
    return len;
}