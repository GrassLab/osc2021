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