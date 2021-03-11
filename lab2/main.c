#include "include/uart.h"
#include "include/shell.h"
#include "include/initrd.h"

extern volatile unsigned char _binary_ramdisk_start;

void main()
{
    extern char __text_start;
    // set up serial console
    uart_init();
    // say hello
    uart_puts("Hello World!\n");
     initrd_list((char*)&_binary_ramdisk_start);
    //uart_puts(&__text_start);
    // echo everything back
    while(1) {
        shell();
    }
}
