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
    int addr = my_alloc(1<<27);
    uart_printf("start to free");
    my_free(addr);
    // say hello
    mem_status_dump();
    //mem_status_dump();
    uart_puts("Hello World!\n");
    // echo everything back
    while(1) {
        shell();
    }
}
