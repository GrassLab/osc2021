#include "systemcall.h"

void int2str(unsigned long num, char *str, int str_len) {
    for (int i = 0; num && i < str_len ; num /=10) {
        str[i] = (num % 10) + '0';
    }
}

int int_len(unsigned long num) {
    int len = 0;
    if (num == 0) return 1;
    for (; num; num /= 10) {
        len++;
    }
    return len;
}

void ptr2str(unsigned long ptr, char *str) {
    for (int i = 0; i < 10; i++) {
        str[i] = '0';
    }
    str[1] = 'x';

    for (int i = 9; ptr && i > 1; i--, ptr /= 16) {
        int num = ptr % 16;
        str[i] += num;
        if (num > 9)
            str[i] += 'a' - ('9' + 1);
    }
}

void delay(int seconds) {
    unsigned long start = time();
    while ((time() - start) < seconds);
    return;
}