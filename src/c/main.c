#include "shell.h"
#include "uart.h"
#include "printf.h"
#include "mm.h"

int main()
{
    init_uart();
    init_memory();
    init_printf(0, putc);

    printf("Hello World!\n\n");
    shell_start();

    return 0;
}