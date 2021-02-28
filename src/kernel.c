#include "mini_uart.h"
#include "shell.h"

void kernel_start(void){
    uart_init();

    simple_shell();
}