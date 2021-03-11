#include "uart.h"
#include "utils.h"

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


void hello() {
    uart_puts("Hello World!\n");
}

void help() {
    uart_puts("help: print all available commands\n");
    uart_puts("hello: print Hello World!\n");
}

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
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


void loadimg() {

    // read the kernel size    
    char input[20];
    uart_read_line(input, 0);
    uart_puts("\r");
    int kernel_size = atoi(input);

    char s[20];
    itoa(kernel_size, s);
    uart_puts("Kernel size is: ");
    uart_puts(s);
    uart_puts("\n");

    // read kernel image then save to 0x80000
    char *new_address = (char *)0x80000;
    int checksum = 0;
    for(int i = 0; i < kernel_size; i++) {
        unsigned char c = uart_getc();
        checksum += c;
        new_address[i] = c;
    }
    
    char checksum_s[20];
    itoa(checksum, checksum_s);
    uart_puts("Loading done! Checksum is: ");
    uart_puts(checksum_s);
    uart_puts("\n");
    

    void (*jump_to_new_kernel)(void) = new_address;
    jump_to_new_kernel();
}


void main() {
    uart_init();
    char *welcome = "\\                             .       .\n \\                           / `.   .\' \" \n \\                  .---.  <    > <    >  .---.\n   \\                 |   \\  \\ - ~ ~ - /  /    |\n         _____          ..-~             ~-..-~\n        |     |  \\~~~\\.\'                    `./~~~/\n       ---------  \\__/                        \\__/\n      .\'  O    \\     /               /       \\  \" \n     (_____,    `._.\'               |         }  \\/~~~/\n      `----.          /       }     |        /    \\__/\n            `-.      |       /      |       /      `. ,~~|\n                ~-.__|      /_ - ~ ^|      /- _      `..-\'   \n                     |     /        |     /     ~-.     `-. _  _  _\n                     |_____|        |_____|         ~ - . _ _ _ _ _>\n";
    //char *welcome = "hi\n";
    //char *welcome = "hello\n";
    uart_puts(welcome);

    while(1) {
        uart_puts("#");
        char input[20];
        uart_read_line(input, 1);
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
        else if(!strcmp(input, "loadimg")) {
            loadimg();
        }
        else {
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }
}
