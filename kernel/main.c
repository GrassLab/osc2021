#include "uart.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (volatile unsigned int*)0x3F10001c
#define PM_WDOG (volatile unsigned int*)0x3F100024

int strcmp(char *a, char *b) {
    while(*a != '\0') {
        if(*a != *b) return 1;
        a++;
        b++;
    }
    if(*a != *b) return 1;
    return 0;
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

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC =  PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
}

void cancel_reset() {
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}

void reboot() {
    uart_puts("Rebooting...\n");
    reset(100);
}

void main() {
    uart_init();
    char *welcome = "\\                             .       .\n \\                           / `.   .\' \" \n \\                  .---.  <    > <    >  .---.\n   \\                 |   \\  \\ - ~ ~ - /  /    |\n         _____          ..-~             ~-..-~\n        |     |  \\~~~\\.\'                    `./~~~/\n       ---------  \\__/                        \\__/\n      .\'  O    \\     /               /       \\  \" \n     (_____,    `._.\'               |         }  \\/~~~/\n      `----.          /       }     |        /    \\__/\n            `-.      |       /      |       /      `. ,~~|\n                ~-.__|      /_ - ~ ^|      /- _      `..-\'   \n                     |     /        |     /     ~-.     `-. _  _  _\n                     |_____|        |_____|         ~ - . _ _ _ _ _>\n";
    uart_puts(welcome);

    while(1) {
        uart_puts("#");
        char input[20];
        uart_read_line(input);
        uart_puts("\r");

        if(!strcmp(input, "hello")) {
            hello();
        }
        else if(!strcmp(input, "help")) {
            help();
        }
        else if(!strcmp(input, "reboot")) {
            reboot();
        }
        else {
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }
}
