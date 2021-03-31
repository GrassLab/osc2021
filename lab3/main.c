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
    mem_status_dump();
    int addr = my_alloc(32);
    dy_mem_status_dump();
    my_free(addr);
    dy_mem_status_dump();
    uart_puts("Hello World!\n");
    // echo everything back
    while(1) {
        shell();
    }
}
