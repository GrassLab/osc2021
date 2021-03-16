#include "include/uart.h"
#include "include/shell.h"
#include "include/initrd.h"

extern volatile unsigned char _binary_initramfs_cpio_start;

void main()
{
    // set up serial console
    uart_init();
    // say hello
    uart_puts("Hello World!\n");
    initrd_list((char*)&_binary_initramfs_cpio_start);
    //uart_puts(&__text_start);
    // echo everything back
    while(1) {
        shell();
    }
}
