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
