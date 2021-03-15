#include "shell.h"
#include "uart.h"
#include "printf.h"

int main()
{
    // set up serial console
    uart_init();
    init_printf(0, putc);
    // say hello
    printf("Hello World!\n");
    // start shell
    shell_start();

    return 0;
}