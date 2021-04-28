#include "uart.h"
#include "utils.h"
#include "string.h"
#include "allocator.h"
#include "cpio.h"
#include "xcpt_func.h"
#include "demo_func.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (volatile unsigned int*)0x3F10001c
#define PM_WDOG (volatile unsigned int*)0x3F100024


void hello() {
    uart_puts("Hello World!\n");
}

void help() {
    uart_puts("help: print all available commands\n");
    uart_puts("hello: print Hello World!\n");
    uart_puts("reboot: reboot\n");
    uart_puts("ls: list all file\n");
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

/*
int getpid(){
	long ret;
	asm volatile("\
		mov w8, 1 \n\
		svc #0 \n");
	return;
}
*/

void main() {
    uart_init();
    init_buckets();

    char *welcome = "\\                             .       .\n \\                           / `.   .\' \" \n \\                  .---.  <    > <    >  .---.\n   \\                 |   \\  \\ - ~ ~ - /  /    |\n         _____          ..-~             ~-..-~\n        |     |  \\~~~\\.\'                    `./~~~/\n       ---------  \\__/                        \\__/\n      .\'  O    \\     /               /       \\  \" \n     (_____,    `._.\'               |         }  \\/~~~/\n      `----.          /       }     |        /    \\__/\n            `-.      |       /      |       /      `. ,~~|\n                ~-.__|      /_ - ~ ^|      /- _      `..-\'   \n                     |     /        |     /     ~-.     `-. _  _  _\n                     |_____|        |_____|         ~ - . _ _ _ _ _>\n";
    uart_puts(welcome);
    while(1) {
        uart_puts("#");
        char input[20];
        uart_read_line(input, 1);
        uart_send('\r');

        if(!strcmp(input, "hello")) {
            hello();
        }
        else if(!strcmp(input, "help")) {
            help();
        }
        else if(!strcmp(input, "reboot")) {
            reboot();
        }
        else if(!strcmp(input, "ls")) {
            list_file();
        }
        else if(!strcmp(input, "lab3")) {
            lab3();
        }
        else if(!strcmp(input, "load")) {
            load_user_program();
        }
        else if(!strcmp(input, "timer")) {
            core_timer_enable();
            from_el1_to_el0();
        }
        else if(!strcmp(input, "lab5-1")) {
            lab5_required_1(2);
        }
        else if(!strcmp(input, "lab5-2")) {
            core_timer_enable();
            lab5_required_2();
            //from_el1_to_el0();
            //int i = getpid();
        }
        else {
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }
}

