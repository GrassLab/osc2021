#include "../include/stringUtils.h"
int compString(char *command, char *buffer){
    unsigned char c1, c2;

    do {
        c1 = (unsigned char)*buffer++;
        c2 = (unsigned char)*command++;
        if (c1 == '\0') {
            return c1 - c2;
        }
    } while (c1 == c2);

    return c1 - c2;


}
