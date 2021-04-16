#include "uart.h"
#include "utils.h"
#include "string.h"

void getFileData(char *target) {
    uart_puts("Please enter file name: ");
    uart_read_line(target, 1);
    uart_send('\r');
    //volatile unsigned char *cpio_address = (unsigned char *) 0x20000000;
    volatile unsigned char *cpio_address = (unsigned char *) 0x8000000;
    
    int i = 0;
    while(1) {
        int file_size = 0;
        int name_size = 0;
        cpio_address += 54;
        file_size = atoi(subStr(cpio_address, 8), 16);
        cpio_address += 40;
        name_size = atoi(subStr(cpio_address, 8), 16);

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
    //volatile unsigned char *cpio_address = (unsigned char *) 0x20000000;
    volatile unsigned char *cpio_address = (unsigned char *) 0x8000000;
    
    int i = 0;
    while(1) {
        int file_size = 0;
        int name_size = 0;
        cpio_address += 54;
        file_size = atoi(subStr(cpio_address, 8), 16);
        cpio_address += 40;
        name_size = atoi(subStr(cpio_address, 8), 16);

        name_size += (name_size+110) % 4 != 0 ? 4 - (name_size+110) % 4 : 0;
        file_size += file_size % 4 != 0 ? 4 - file_size % 4 : 0;
        
        cpio_address += 16;

        char *path_name = cpio_address;
        if(!strcmp(path_name, "TRAILER!!!")) {
            break;
        }
        uart_puts(path_name);
        uart_puts("\n");

        cpio_address += name_size;
        unsigned char *file_data = cpio_address;
        cpio_address += file_size;
    }
    char target[100];
    getFileData(target);
}

void load_user_program() {
    char target[100];
    uart_puts("Please enter file name: ");
    uart_read_line(target, 1);
    uart_send('\r');
    volatile unsigned char *cpio_address = (unsigned char *) 0x20000000;
    //volatile unsigned char *cpio_address = (unsigned char *) 0x8000000;
    volatile unsigned char *prog_addr = (unsigned char *) 0x100000;
    
    int i = 0;
    while(1) {
        int file_size = 0;
        int name_size = 0;
        cpio_address += 54;
        file_size = atoi(subStr(cpio_address, 8), 16);
        cpio_address += 40;
        name_size = atoi(subStr(cpio_address, 8), 16);

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
                prog_addr[i] = file_data[i];
            }
            uart_puts("loading...\n");
            asm volatile("bl eret_user");
        }
        cpio_address += file_size;
    }
}