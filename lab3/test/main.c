#include "uart.h"
#include "utils.h"
#include "reboot.h"
#include "page_malloc.h"
#include "string.h"
void test();
void main() {
    uart_init();    
    print_welcome(1);
    char input[20];
    char tmp_string[20];

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
        else if(!strcmp(input, "loadimg")) {
            loadimg();
        }
        else if(!strcmp(input, "ls")) {
            ls(input,0);
        }
        else if(!strcmp(input, "demo")) {
            alloc_page_init(0x10000000-0x10, 0x20000000+0x10);           
            unsigned long p0 = alloc_page(4096);
            unsigned long p1 = alloc_page(4096);
            unsigned long p2 = alloc_page(4096);
            unsigned long p3 = alloc_page(4096);
            free_page(p2, 4096);
            free_page(p1, 4096);
            free_page(p3, 4096);
            free_page(p0, 4096);
            p0 = alloc_page(16);
            p1 = alloc_page(16);
            p2 = alloc_page(16);
            p3 = alloc_page(16);
            
            free_page(p2, 16);
            free_page(p1, 16);
            free_page(p0, 16);
            free_page(p3, 16);
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
        else if(!strcmp(input, "test")) {
            test();
        }
        else {
            if(!ls(input,1)){
                uart_puts("Error: ");
                uart_puts(input);
                uart_puts(" command not found! Try <help> to check all available commands\n");
            }            
        }
    }
}

