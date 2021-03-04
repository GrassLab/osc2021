#include "uart.h"
#include "shell.h"

void main(){
    // set up serial console
    uart_init();
    
    // uart_puts
    uart_puts("rpi3 is already\n");

    // start shell
    shell_start();

}
