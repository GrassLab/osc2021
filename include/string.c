#include "string.h"

int strlength (char *str) {
    int num = 0;
    while (str[num]) num++;
    return num;
}

int strcmp (char *str1, char *str2) {
    for (; *str1 && *str2; str1++, str2++) {
        if (*str1 > *str2)
            return -1;
        else if (*str1 < *str2)
            return 1;
    }
    if (*str1)
        return -1;
    else if (*str2)
        return 1;
    return 0;
}

void strncopy (char *d, char *r, unsigned int len) {
    unsigned long i;
    for (i = 0; i < len - 1 && r[i] != '\0' ; i++)
        d[i] = r[i];

    d[i] = '\0';
}

/* translate hex format string to unsigned integer */
unsigned long htoui (char *b) {
    if (b[0] != '0' || b[1] != 'x')
        return 0;

    unsigned long num = 0;
    for (int i = 2; b[i] != '\0'; i++) {
        num *= 16;
        if ('0' <= b[i] && b[i] <= '9')
            num += b[i] - '0';
        else if ('a' <= b[i] && b[i] <= 'f')
            num += b[i] - 'a' + 10;
        else if ('A' <= b[i] && b[i] <= 'F')
            num += b[i] - 'A' + 10;
        /* wrong hex format */
        else
            return 0;
    }
    return num;
}

/* translate string to unsigned integer */
unsigned long atoui (char *b) {
    if (b[0] == '0' && b[1] == 'x')
        return htoui(b);

    unsigned long num = 0;
    for (int i = 0; b[i] != '\0'; i++) {
        num *= 10;
        if ('0' <= b[i] && b[i] <= '9')
            num += b[i] - '0';
        /* wrong hex format */
        else
            return 0;
    }
    return num;
}

/* translate string to integer */
long atoi (char *b) {
    if (b[0] != '-')
        return (int)atoui(b);

    long num = 0;
    for (int i = 1; b[i] != '\0'; i++) {
        num *= 10;
        if ('0' <= b[i] && b[i] <= '9')
            num += b[i] - '0';
        /* wrong hex format */
        else
            return 0;
    }
    return -num;
}

