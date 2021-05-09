#include "uart.h"
#include "reboot.h"
#include "utils.h"
#include "memory.h"

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
    char *str;
    
    while(1) {
        uart_puts("#");
        input(buf);
        uart_send('\r');
        
        slice_str(buf, cat, filename, 3);
        if(!strcmp(buf, hello)) {
            uart_puts("Hello World!\n");
        }
        else if(!strcmp(buf, help)) {
            uart_puts("hello: print Hello World!\n");
            uart_puts("help: print all available commands\n");
            uart_puts("reboot: reboot\n");
            uart_puts("ls: filenames in current directory\n");
            uart_puts("cat filename: display the file\n");
        }
        else if(!strcmp(buf, reboot)) {
            reset(1000);
        }
        else if (!strcmp(buf, "demo_frame")) {
            allocator_init();
            unsigned long *p1 = malloc(4096*3);
            unsigned long *p2 = malloc(4096);
            unsigned long *p3 = malloc(4096);

            free(p1);
            free(p3);
            free(p2);
        }
        else if (!strcmp(buf, "demo")) {
            allocator_init();
            unsigned long *p1 = malloc(8);
            unsigned long *p2 = malloc(17);
            unsigned long *p3 = malloc(2000);       

            free(p1);
            free(p2);
            free(p3);
        }
        else if (!strcmp(buf, "demo2")) {
            allocator_init();
            unsigned long *array[300];
            for (int i = 0; i < 300; i++) {
                array[i] = malloc(8);
            }
            for (int i = 0; i < 300; i++) {
                free(array[i]);
            }
        }
        else if (!strcmp(buf, "ls")) {
            ls("\0", 0);
        }
        else if (!strcmp(cat, "cat")) {
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