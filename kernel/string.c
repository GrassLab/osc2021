#include "string.h"

int strcmp(const char *str1, const char *str2) {
    int i;
    for(i = 0; i < strlen(str1); i++)
        if(str1[i] != str2[i])
            return str1[i] - str2[i];

    return str1[i] - str2[i];
}

int strncmp(const char *a, const char *b, unsigned long n)
{
    unsigned long i;
    for (i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return a[i] - b[i];
        }
        if (a[i] == 0) {
            return 0;
        }
    }
    return 0;
}

int strlen(const char *str) {
    int i = 0;
    while(1) {
        if(str[i] == '\0')
            break;
        i++;
    }

    return i;
}

char* strcpy(char *to, const char *from) {
    char *save = to;
    while (*from != 0) {
        *to = *from;
        to++;
        from++;
    }
    return save;
}

inline void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

// function to reverse buffer[i..j]
char* reverse(char *buffer, int i, int j)
{
    while (i < j)
        swap(&buffer[i++], &buffer[j--]);
 
    return buffer;
}

