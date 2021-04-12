#include "util.h"
#include "string.h"
#include "uart.h"

char* itoa(int value, char* buffer, int base)
{
    // invalid input
    if (base < 2 || base > 32)
        return buffer;
 
    // consider absolute value of number
    int n = value;
 
    int i = 0;
    while (n)
    {
        int r = n % base;
 
        if (r >= 10) 
            buffer[i++] = 65 + (r - 10);
        else
            buffer[i++] = 48 + r;
 
        n = n / base;
    }
 
    // if number is 0
    if (i == 0)
        buffer[i++] = '0';
 
    // If base is 10 and value is negative, the resulting string 
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10)
        buffer[i++] = '-';
 
    buffer[i] = '\0'; // null terminate string
 
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

void itohexstr(uint64_t d, int size, char * s)
{
    int i = 0;
    unsigned int n;
    int c;

    c = size * 8;
    s[0] = '0';
    s[1] = 'x';

    for( c = c - 4, i = 2; c >= 0; c -= 4, i++)
    {
        // get highest tetrad
        n = ( d >> c ) & 0xF;

        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        if ( n > 9 && n < 16 )
            n += ('A' - 10);
        else
            n += '0';
       
        s[i] = n;
    }

    s[i] = '\0';
}

int atoi(const char *str) {
    int res = 0;

    for(int i = 0; str[i] != '\0'; i++)
        res = res * 10 + str[i] - '0';

    return res;
}

int hextoi(const char *str) {
    int res = 0;

    for(int i = 0; str[i] != '\0'; i++) {
        if(str[i] >= 97 && str[i] <= 102)
            res = res * 16 + str[i] - 'a' + 10;
        else
            res = res * 16 + str[i] - '0';
    }

    return res;
}


void print_memory_with_uart(void *address, int size) {
    int printed = 0;
    unsigned char *current = address; 

    for(int i = 0; i < size; i++) {
        int g = (*(current + i) >> 4) & 0xf;
        g += g >= 10 ? 'a' - 10 : '0';

        uart_send(g);
        printed ++;

        g = *(current + i) & 0xf;
        g += g >= 10 ? 'a' - 10 : '0';
        uart_send(g);
        printed ++;

        if(printed % 32 == 0)
            uart_send('\n');
        else if(printed % 4 == 0)
            uart_send(' ');
    }
}