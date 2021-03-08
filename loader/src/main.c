#include "uart.h"

int main()
{
    // set up serial console
    uart_init();

    // say hello
    uart_puts("Hello World!\n");
    
    while (1)
    {
    }
    return 0;
}