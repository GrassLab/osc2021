#include "string.h"

int strcmp(char *str1, char *str2) {
    int i;
    for(i = 0; i < strlen(str1); i++)
        if(str1[i] != str2[i])
            return str1[i] - str2[i];

    return str1[i] - str2[i];
}

int strlen(char *str) {
    int i = 0;
    while(1) {
        if(str[i] == '\0')
            break;
        i++;
    }

    return i;
}

void itohexstr(uint64_t d, int size, char * s)
{
    int i = 0;
    unsigned int n;
    int c;

    c = size * 8;
    s[0] = '0';
    s[1] = 'x';

    for( c = c - 4, i = 2; c >= 0; c -= 4, i++)
    {
        // get highest tetrad
        n = ( d >> c ) & 0xF;

        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        if ( n > 9 && n < 16 )
            n += ('A' - 10);
        else
            n += '0';
       
        s[i] = n;
    }

    s[i] = '\0';
}