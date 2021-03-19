#include "uart.h"
#include "utils.h"
#include "reboot.h"

void main() {
    uart_init();    
    print_welcome();
    
    while(1) {
        uart_puts("#");
        char input[20];
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
        else {
            if(!ls(input,1)){
                uart_puts("Error: ");
                uart_puts(input);
                uart_puts(" command not found! Try <help> to check all available commands\n");
            }            
        }
    }
}

