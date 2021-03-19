#include "shell.h"
#include "uart.h"

void main()
{
    uart_init();

    uart_puts("\nWelcome to Rpi3\n");

    shell();
}