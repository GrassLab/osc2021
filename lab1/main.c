#include "include/uart.h"
#include "include/shell.h"
void main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("Hello World!\n");
    
    // echo everything back
    while(1) {
        shell();
    }
}
