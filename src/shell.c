#include "cpio.h"
#include "mystring.h"
#include "uart.h"
#include "utils.h"

void shell_cmd(char* cmd) {
    if(!strcmp(cmd, "help")) {
        uart_putstr("help: print all available commands\n");
        uart_putstr("hello: print Hello World!\n");
        uart_putstr("reboot: reboot rpi\n");
        uart_putstr("loadimg: start loadimg from uart\n");
        uart_putstr("cpio: read cpio data\n");
    }
    else if(!strcmp(cmd, "hello")) {
        uart_putstr("Hello World!\n");
    }
    else if(!strcmp(cmd, "reboot")) {
        reset(100);
    }
    else if(!strcmp(cmd, "loadimg")) {
        loadimg();
    }
    else if(!strcmp(cmd, "cpio")) {
        print_cpio();
    }
    else if(!strcmp(cmd, "")) {
    }
    else {
        uart_putstr("Unknwon command!\n");
    }
}
