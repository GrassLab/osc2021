#include "uart.h"
#include "reboot.h"
#include "utils.h"

int main() {
    uart_init();
    char *helloworld = "\n _   _      _ _         __        __         _     _ \n\
| | | | ___| | | ___    \\ \\      / /__  _ __| | __| |\n\
| |_| |/ _ \\ | |/ _ \\    \\ \\ /\\ / / _ \\| '__| |/ _` |\n\
|  _  |  __/ | | (_) |    \\ V  V / (_) | |  | | (_| |\n\
|_| |_|\\___|_|_|\\___( )    \\_/\\_/ \\___/|_|  |_|\\__,_|\n\
                    |/                               \n";
    uart_puts(helloworld);
    char buf[20];
    char *help = "help";
    char *hello = "hello";
    char *reboot = "reboot";
    char cat[4];
    char filename[10];
    
    while(1) {
        uart_puts("#");
        input(buf);
        slice_str(buf, cat, filename, 3);
        if(strcmp(buf, hello)) {
            uart_puts("Hello World!\n");
        }
        else if(strcmp(buf, help)) {
            uart_puts("hello: print Hello World!\n");
            uart_puts("help: print all available commands\n");
            uart_puts("reboot: reboot\n");
            uart_puts("ls: filenames in current directory\n");
            uart_puts("cat filename: display the file\n");
        }
        else if(strcmp(buf, reboot)) {
            reset(1000);
        }
        else if (strcmp(buf, "ls")) {
            ls("\0", 0);
        }
        else if (strcmp(cat, "cat")) {
            if(!ls(filename, 1)) {
                uart_puts("Invalid pathname!!\n");
            }
        }
        else {
            uart_puts("Error: ");
            uart_puts(buf);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }
    return 0;
}