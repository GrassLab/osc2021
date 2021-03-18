#include "uart.h"
#include "reboot.h"

// refer to: https://www.geeksforgeeks.org/write-your-own-atoi/
int myAtoi(char* str)
{
    // Initialize result
    int res = 0;
 
    // Iterate through all characters
    // of input string and update result
    // take ASCII character of corosponding digit and
    // subtract the code from '0' to get numerical
    // value and multiply res by 10 to shuffle
    // digits left to update running total
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';
 
    // return result.
    return res;
}

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


int main() {
    uart_init();
    uart_puts("uart initialize successful!\n");
    char buf[10];
    char *help = "help";
    char *hello = "hello";
    char *reboot = "reboot";
    char *load_img = "load_img";
    char tmp = 'a';
    char *img_size;
    int size;
    int idx;
    
    while(1) {
        uart_puts("#");
        input(buf);
        if(strcmp(buf, hello)) {
            uart_puts("Hello World!\n");
        }
        else if(strcmp(buf, help)) {
            uart_puts("hello: print Hello World!\n");
            uart_puts("help: print all available commands\n");
            uart_puts("reboot: reboot\n");
            uart_puts("load_img: load image from host machine\n");
        }
        else if(strcmp(buf, reboot)) {
            reset(1000);
            uart_puts("press c to cancel reboot\n");
            input(buf);
            if(strcmp(buf, "c")) {
                cancel_reset();
            }
        }
        else if(strcmp(buf, load_img)) {
            idx = 0;
            while(1) {
                tmp = uart_getc();
                if(tmp != '\n') {
                    img_size[idx] = tmp;
                    idx++;
                }
                else {
                    img_size[idx] = '\0';
                    break;
                }
            }
            //uart_puts(img_size);
            
            size = myAtoi(img_size)*10;
            volatile unsigned char *kernel = (void *)(long)0x80000;
            //uart_puts("\n");

            uart_puts("Start loading kernel...\n");
            
            
            idx = 0;

            while(size) {
                kernel[idx] = uart_getc();
                idx++;
                size--;
            }
            uart_puts("Complete loading\n");
            kernel = (void *)(long)0x80000;
            asm volatile("br %0" : "=r"((unsigned long int*)kernel));           
            
        }
        else {
            uart_puts("Unrecognized command: ");
            uart_puts(buf);
            uart_puts("\nUse <help> to find commands\n");
        }
    }
}