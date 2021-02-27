#include "uart.h"

int strcmp(char *a, char *b) {
    while(*a != '\0') {
        if(*a != *b) return 0;
        a++;
        b++;
    }
    if(*a != *b) return 0;
    return 1;
}

void uart_read_line(char *input) {
    int index = 0;
    char c;
    while(c != '\n') {
        c = uart_getc();
        uart_send(c);
        if(c != '\n') {
            input[index] = c;
            index++;
        }
        else {
            input[index] = '\0';
        }
    }
}

void hello() {
    uart_puts("Hello World!\n");
}

void help() {
    uart_puts("help: print all available commands\n");
    uart_puts("hello: print Hello World!\n");
}

void main() {
    uart_init();
    char* welcome = "\n                        ___                  _ _ _        \n                       |__ \\                (_|_) |       \n   _ __ _   _ _ __ ___    ) |_ __ ___   ___  _ _| |_ ___  \n  | '__| | | | '_ ` _ \\  / /| '_ ` _ \\ / _ \\| | | __/ _ \\ \n  | |  | |_| | | | | | |/ /_| | | | | | (_) | | | || (_) |\n  |_|   \\__,_|_| |_| |_|____|_| |_| |_|\\___/| |_|\\__\\___/ \n                                           _/ |           \n                                          |__/            \n";
    uart_puts(welcome);

    while(1) {
        uart_puts("#");
        char input[20];
        uart_read_line(input);
        uart_puts("\r");

        if(strcmp(input, "hello")) {
            hello();
        }
        else if(strcmp(input, "help")) {
            help();
        }
        else {
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts("command not found! Try <help> to check all available commands\n");
        }
    }
}