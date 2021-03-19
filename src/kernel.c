#include "mini_uart.h"
#include "shell.h"
#include "device_tree.h"

void kernel_start(void){
    uart_init();

    // parse_dt();

    simple_shell();
}