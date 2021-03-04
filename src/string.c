#include "string.h"

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
