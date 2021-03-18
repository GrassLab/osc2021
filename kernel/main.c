#include "uart.h"
#include "utils.h"
#include "string.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (volatile unsigned int*)0x3F10001c
#define PM_WDOG (volatile unsigned int*)0x3F100024


void hello() {
    uart_puts("Hello World!\n");
}

void help() {
    uart_puts("help: print all available commands\n");
    uart_puts("hello: print Hello World!\n");
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


void getFileData(char *target) {
    uart_puts("Please enter file name: ");
    uart_read_line(target, 1);
    uart_send('\r');
    volatile unsigned char *cpio_address = (unsigned char *) 0x20000000;
    
    int i = 0;
    while(1) {
        int file_size = 0;
        int name_size = 0;
        cpio_address += 54;
        file_size = atoi(subStr(cpio_address, 8), 16);
        cpio_address += 40;
        name_size = atoi(subStr(cpio_address, 8), 16);

        /*
        char *s;
        itoa(name_size, s);
        uart_puts(s);
        uart_puts("\n");

        char *s1;
        itoa(file_size, s1);
        uart_puts(s1);
        uart_puts("\n");
        */

        name_size += (name_size+110) % 4 != 0 ? 4 - (name_size+110) % 4 : 0;
        file_size += file_size % 4 != 0 ? 4 - file_size % 4 : 0;
        
        cpio_address += 16;

        char *path_name = cpio_address;
        if(!strcmp(path_name, "TRAILER!!!")) {
            uart_puts("No such file\n");
            break;
        }

        cpio_address += name_size;
        unsigned char *file_data = cpio_address;
        if(!strcmp(path_name, target)) {
            for(int i = 0; i < file_size; i++) {
                if(file_data[i] == '\n') {
                    uart_send('\r');
                }
                uart_send(file_data[i]);
                
            }
            uart_puts("\n");
            break;
        }
        cpio_address += file_size;
    }
}

void list_file() {
    volatile unsigned char *cpio_address = (unsigned char *) 0x20000000;
    
    int i = 0;
    while(1) {
        int file_size = 0;
        int name_size = 0;
        cpio_address += 54;
        file_size = atoi(subStr(cpio_address, 8), 16);
        cpio_address += 40;
        name_size = atoi(subStr(cpio_address, 8), 16);

        /*
        char *s;
        itoa(name_size, s);
        uart_puts(s);
        uart_puts("\n");

        char *s1;
        itoa(file_size, s1);
        uart_puts(s1);
        uart_puts("\n");
        */

        name_size += (name_size+110) % 4 != 0 ? 4 - (name_size+110) % 4 : 0;
        file_size += file_size % 4 != 0 ? 4 - file_size % 4 : 0;
        
        cpio_address += 16;

        char *path_name = cpio_address;
        if(!strcmp(path_name, "TRAILER!!!")) break;
        uart_puts(path_name);
        uart_puts("\n");

        cpio_address += name_size;
        unsigned char *file_data = cpio_address;
        //for(int i = 0; i < file_size; i++) {
        //    uart_send(file_data[i]);
        //}
        //uart_puts("\n");

        cpio_address += file_size;
    }
    char target[100];
    getFileData(target);
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
        else {
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }
}
