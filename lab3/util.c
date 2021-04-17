#include "util.h"

unsigned int big_to_little_32(unsigned int b_num) {
    unsigned int l_num = 0;
    for (int i = 0; i < 4; i++) {
        l_num = l_num << 8;
        l_num += b_num & 0xff;
        b_num = b_num >> 8;
    }
    return l_num;
}

unsigned long big_to_little_64(unsigned long b_num) {
    unsigned int l_num = 0;
    for (int i = 0; i < 8; i++) {
        l_num = l_num << 8;
        l_num += b_num & 0xff;
        b_num = b_num >> 8;
    }
    return l_num;
}

unsigned long align4(unsigned long num) {
    if (num == 0)
        return 0;
    return (((num - 1) / 4) + 1) * 4;
}