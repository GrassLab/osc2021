#include "include/uart.h"
#include "include/shell.h"
#include "include/initrd.h"
#include "include/memAlloc.h"


void main()
{
    // set up serial console
    uart_init();
    //memory allocator init
    mem_init();
    // say hello
    uart_puts("Hello World!\n");
    // echo everything back
    while(1) {
        shell();
    }
}
