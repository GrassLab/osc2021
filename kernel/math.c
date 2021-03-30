#include "math.h"

double log10(double x) {
    return ln(x) / LN10;    
}

double ln(double x)
{
    double sum = 0.0;
    double xmlxpl = (x - 1) / (x + 1);
    double denom = 1.0;
    double frac = xmlxpl;
    double term = frac / denom;

    while (term > ALMOSTZERO)
    {
        sum += term;
        //generate next term
        denom += 2.0;
        frac = frac * xmlxpl * xmlxpl;
        term = frac / denom;
    }
    return 2.0 * sum;
}

float log(double base, double x) {
    return log10(x) / log10(base);
}

int pow(int base, int exponent) {
    int result = 1;
    for(int e = exponent; e > 0; e--) 
        result = result * base;
    
    return result;
}


int ceil(double x) {
    if(x == (int)x) 
        return (int)x;
    else 
        return ((int)x) + 1;
}