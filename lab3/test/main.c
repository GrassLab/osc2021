#include "uart.h"
#include "utils.h"
#include "reboot.h"
#include "page_malloc.h"
#include "string.h"

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
        else if(!strcmp(input, "alloc")) {
            alloc_page_init(0x10000000, 0x20000000);           
            alloc_page(4096);
            alloc_page(4096);
            alloc_page(4096);
            alloc_page(4096);
            alloc_page(4096);
            alloc_page(4096);
            alloc_page(4096);
            alloc_page(4096);
            free_page(0x1FC00000+4096*6, 4096);
            free_page(0x1FC00000+4096*5, 4096);
            free_page(0x1FC00000+4096*4, 4096);
            free_page(0x1FC00000+4096*7, 4096);
        }
        else if(!strcmp(input, "alloc_page")) {
            uart_puts("allocated page at: ");
            dec_hex(alloc_page(4096*2*2*2),tmp_string);
            uart_puts("\n");
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

