#include "string.h"
#include "uart.h"

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