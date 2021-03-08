#include "string.h"

int strcmp(const char *a, const char *b){
    if(!(*a | *b)) return 0;
    return (*a != *b)? *a - *b : strcmp(++a, ++b);
}