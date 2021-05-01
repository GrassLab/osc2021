#include "uart.h"
#include "utils.h"
#include "reboot.h"
#include "page_malloc.h"
#include "string.h"
#include "thread.h"

void main() {
    print_welcome(1);
    char input[20];
    alloc_page_init(0x10000000, 0x20000000);
    while(1) {
        uart_puts("#");
        
        shell(input);

        if(!strcmp(input, "hello")) {
            uart_puts("Hello World!\n");
        }
        else if(!strcmp(input, "help")) {
            uart_puts("hello: print Hello World!\n");
            uart_puts("help: print all available commands\n");
            uart_puts("reboot: reboot system\n");
        }
        else if(!strcmp(input, "reboot")) {
            reset(20000);
            uart_puts("input c to cancel reset\n");
            shell(input);
            if(!strcmp(input, "c")) {
                cancel_reset();
            }
        }
        else if(!strcmp(input, "xcpt")) {            
            from_el1_to_el0(0x3c0);
            xcpt_test();
        }
        else if(!strcmp(input, "time")) {                        
            core_timer_enable();
            from_el1_to_el0(0x0);            
        }
        else if(!strcmp(input, "loadimg")) {
            loadimg();
        }
        else if(!strcmp(input, "ls")) {
            ls(input,0);
        }
        else if(!strcmp(input, "alloc_init")) {
            uart_puts("input low  address(hex): ");
            shell(input);
            unsigned int addr_low = atoi(input, 16);
            uart_puts("input high address(hex): ");
            shell(input);
            unsigned int addr_high = atoi(input, 16);
            alloc_page_init(addr_low, addr_high);
        }
        else if(!strcmp(input, "alloc_page")) {
            uart_puts("input page size: ");
            shell(input);
            unsigned int page_size = atoi(input, 10);
            alloc_page(page_size);
        }
        else if(!strcmp(input, "free_page")) {
            uart_puts("input page address(hex): ");
            shell(input);
            unsigned int page_address = atoi(input, 16);
            uart_puts("input page size: ");
            shell(input);
            unsigned int free_size = atoi(input, 10);
            free_page(page_address, free_size);
        }
        else if(!strcmp(input,"lab5-1")){
			threadTest1();
		}else if(!strcmp(input,"lab5-2")){
			threadTest2();
		}
        else {
            //if(!ls(input,1))
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts(" command not found! Try <help> to check all available commands\n");           
        }
    }
}

