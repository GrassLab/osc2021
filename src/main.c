#include "uart.h"
#include "shell.h"
#include "driver.h"
#include "dtb_parser.h"
#include "buddy.h"

void main(void){
    uart_init();
    buddy_init();
    buddy_test1();
    // while(uart_get() == '\0');
    shell();    
}
