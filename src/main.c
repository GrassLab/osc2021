#include "uart.h"
#include "shell.h"
#include "driver.h"
#include "dtb_parser.h"

void main(void){
    uart_init();
    // while(uart_get() == '\0');
    shell();    
}
