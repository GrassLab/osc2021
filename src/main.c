#include "uart.h"
#include "shell.h"
#include "driver.h"
#include "dtb_parser.h"
#include "buddy.h"
#include "dynamic_allocator.h"

void main(void){
    DEBUG = 0;
    uart_init();
    buddy_init();
    dynamic_allocator_init();
    
    // while(uart_get() == '\0');
    shell();    
}
