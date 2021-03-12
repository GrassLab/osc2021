#include "uart.h"
#include "shell.h"

void main() {
    uart_init();

    shell_welcome_message();

    while(1) {
        shell_start();
    } 
}