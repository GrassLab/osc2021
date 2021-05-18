#include "uart.h"
#include "reboot.h"


int strcmp(char *a,char *b) {
    while(*a != '\0') {
        if (*a != *b) return 0;
        else {
            a++;
            b++;
        }
    }
    if (*a != *b) return 0;
    return 1;
}

void input(char *command) {
    int index = 0;
    char tmp;
    do {
        tmp = uart_getc();
        command[index] = (tmp != '\n')? tmp : '\0';
        uart_send(tmp);
        index++;
    } while(tmp != '\n');
}


int main() {
    uart_init();
    uart_puts("--------------------lab1--------------------\n");
    char buf[10];
    char *help = "help";
    char *hello = "hello";
    char *reboot = "reboot";
    
    while(1) {
        input(buf);
        if(strcmp(buf, hello)) {
            uart_puts("Hello World!\n");
        }
        else if(strcmp(buf, help)) {
            uart_puts("hello: print Hello World!\n");
            uart_puts("help: print all available commands\n");
            uart_puts("reboot: reboot\n");
        }
        else if(strcmp(buf, reboot)) {
            reset(1000);
        }
        else {
            uart_puts("Error: ");
            uart_puts(buf);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }
}