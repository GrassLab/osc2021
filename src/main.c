#include "uart.h"
#include "shell.h"
#include "driver.h"
#include "dtb_parser.h"
#include "buddy.h"
#include "dynamic_allocator.h"

void main(void){
    uart_init();
    buddy_init();
    // buddy_test1();
    dynamic_allocator_init();
    DMA_test();
    // while(uart_get() == '\0');
    shell();    
}
