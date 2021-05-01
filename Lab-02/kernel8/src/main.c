#include "uart.h"
#include "shell.h"


void kernel_main(){
    // set up serial console
    uart_init();
    // start shell
    shell();
}
