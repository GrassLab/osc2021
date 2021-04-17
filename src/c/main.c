#include "shell.h"
#include "uart.h"
#include "printf.h"
#include "mm.h"
#include "timer.h"

int main()
{
    init_uart();
    init_printf(0, putc);
    init_memory();
    init_timer();

    printf("Hello World!\n\n");
    shell_start();

    return 0;
}