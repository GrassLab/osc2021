#include "uart.h"
#include "shell.h"

void main(void){
    uart_init();
    // while(uart_get() == '\0');
    shell();    
}
