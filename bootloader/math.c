#include "math.h"

int pow(int base, int exponent)
{
    int result = 1;
    for (; exponent > 0; --exponent)
    {
        result *= base;
    }
    
    return result;
}

int log(int base, int val)
{
    int result = 0;
    while(val > 1)
    {
        result++;
        val /= base;
    }

    return result;
}
