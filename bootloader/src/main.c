#include "uart.h"
#include "shell.h"


void bootloader(){
    // set up serial console
    uart_init();
    // start shell
    shell();
}
