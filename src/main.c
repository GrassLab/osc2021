#include "shell.h"
#include "uart.h"
#include "printf.h"
#include "mm.h"

int main()
{
    uart_init();
    uart_puts("Hello World!\n");
    // init_printf(0, putc);
    // printf("Hello World!\n\n");
    shell_start();

    // printf("--------------------\n\n");

    // init_memory();

    // void *address_1 = km_allocation(16);
    // void *address_2 = km_allocation(64);
    // km_free(address_1);
    // void *address_3 = km_allocation(1024);
    // km_free(address_2);
    // km_free(address_3);
    // void *address_4 = km_allocation(16);
    // km_free(address_4);
    // void *address_5 = km_allocation(32);
    // void *address_6 = km_allocation(32);
    // km_free(address_5);
    // km_free(address_6);
    // void *address_7 = km_allocation(512);
    // void *address_8 = km_allocation(512);
    // void *address_9 = km_allocation(16384);
    // km_free(address_8);
    // km_free(address_7);
    // void *address_10 = km_allocation(8192);
    // km_free(address_9);
    // km_free(address_10);

    // printf("DONE!!!!!\n");

    return 0;
}