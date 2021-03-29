#ifndef _LCD_MATH_H_
#define _LCD_MATH_H_

#define ALMOSTZERO 0.0000000000000000001
#define LN10 2.3025850929940456840179914546844

double log10(double x);
double ln(double x);
double log(double base, double x);

int pow(int base, int exponent);

int ceil(double x);

#endif