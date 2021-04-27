#include "uart.h"
#include "shell.h"
#include "driver.h"
#include "dtb_parser.h"
#include "buddy.h"
#include "dynamic_allocator.h"
#include "printf.h"

void main(void){
    DEBUG = 0;
    init_timer_queue();
    uart_init();
    buddy_init();
    dynamic_allocator_init();
    init_thread();
    //printf("hahaha %d\n", 123);
    // core_timer_enable();
    // while(uart_get() == '\0');
    shell();    
}
