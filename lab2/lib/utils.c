#include "uart.h"

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

int slice_str(char *input, char *output, char *filename, int length) {
    for (int i = 0; i < length; i++) {
        if (input[i] == '\0') {
            output[i] = '\0';
            return 0;
        }
        else {
            output[i] = input[i];
        }
        if (i == length-1) output[i+1] = '\0';
    }
    

    int index = 0;
    while (1) {
        if (!input[length+1+index]) {
            filename[index] = '\0';
            return 1;
        }
        filename[index] = input[length+1+index];
        index++;
    }
}

int atoi(char* s) {
    int i = 0;
    int num = 0;
    int negative = 0;

    while(s[i] != '\0') {
        if(i == 0 && s[i] == '-') {
            negative = 1;
        }
        else {
            int digit = s[i] - '0';
            num = num*10 + digit;
        }
        i++;
    }
    
    if(negative) return num *= -1;
    return num;
}


void itoa(int num, char *s) {
    int i = 0;
    while(num != 0) {
        int digit = num % 10;
        s[i++] = digit + '0';
        num /= 10;
    }
    s[i] = '\0';
    
    // reverse s
    for(int j = i-1; j >= i/2; j--) {
        char temp = s[j];
        s[j] = s[i-1-j];
        s[i-1-j] = temp;
    }
}

int ls(char *input, int readfile) {
    // 0x8000000 for qemu, 0x20000000 for raspi 3
    //volatile unsigned char *kernel = (unsigned char *) 0x8000000;
    volatile unsigned char *kernel = (unsigned char *) 0x20000000;
    volatile unsigned char *filename;
    volatile unsigned char *filedata;
    int header_offset = 0x6E;
    int filesize_offset = 6 + 6*8;
    int namesize_offset = 6 + 11*8;
    int filesize;
    int namesize;

    while (1) {
        // compute file size and name size
        filesize = 0;
        namesize = 0;
        for (int i = 0; i < 8; i++) {
            // 10~16
            if (kernel[filesize_offset+i] >= 'A') {
                filesize = filesize * 0x10 + (kernel[filesize_offset+i] - 'A' + 0xA);
            }
            // 0~9
            else {
                filesize = filesize * 0x10 + (kernel[filesize_offset+i] - '0');
            }

            if (kernel[namesize_offset+i] >= 'A') {
                namesize = namesize * 0x10 + (kernel[namesize_offset+i] - 'A' + 0xA);
            }
            else {
                namesize = namesize * 0x10 + (kernel[namesize_offset+i] - '0');
            }
        }
        namesize += header_offset;

        // padding
        if (filesize % 4 != 0) {
            filesize += (4 - filesize % 4);
        }
        if (namesize % 4 != 0) {
            namesize += (4 - namesize % 4);
        }

        // check if pathname is "TRAILER!!!"
        filename = kernel + header_offset;
        if (strcmp(filename, "TRAILER!!!")) return 0;

        // print filename and ignore root
        if (!strcmp(filename, ".") && !readfile) {
            uart_puts(filename);
            uart_puts("\n");
        }
        // print file data
        else if (strcmp(filename, input) && readfile) {
            filedata = kernel + namesize;
            for (int i = 0; i < filesize; i++) {
                uart_send(filedata[i]);
                if (filedata[i] == '\n') uart_send('\r');
            }
            uart_puts("\n");
            return 1;
        }
        kernel += (filesize + namesize);
    }
}