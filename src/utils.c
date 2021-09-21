#include "printf.h"
#include "utils.h"

void print_0x_64bit(void *p)
{
    printf("0x%x%x\n", (unsigned long)p >> 32, p);
}