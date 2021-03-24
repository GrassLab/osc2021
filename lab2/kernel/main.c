#include "uart.h"
#include "shell.h"

void main()
{
    uart_init();

    uart_put_str("\nwelcome to rpi3\n");
    shell();
}