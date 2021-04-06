#include "shell.h"
#include "uart.h"
#include "printf.h"
#include "memory.h"

int main()
{
    uart_init();
    init_printf(0, putc);
    printf("Hello World!\n\n");

    printf("--------------------\n\n");

    init_memory();

    void *address_1 = km_allocation(16);
    void *address_2 = km_allocation(64);
    km_free(address_1);
    void *address_3 = km_allocation(1024);
    km_free(address_2);
    km_free(address_3);
    void *address_4 = km_allocation(16);
    km_free(address_4);
    void *address_5 = km_allocation(32);
    void *address_6 = km_allocation(32);
    km_free(address_5);
    km_free(address_6);
    void *address_7 = km_allocation(512);
    void *address_8 = km_allocation(512);
    void *address_9 = km_allocation(16384);
    km_free(address_8);
    km_free(address_7);
    void *address_10 = km_allocation(8192);
    km_free(address_9);
    km_free(address_10);

    printf("DONE!!!!!\n");
    // void *address_11 = memory_allocation(8192);
    // void *address_12 = memory_allocation(65536);
    // void *address_13 = memory_allocation(128);
    // memory_free(address_11);
    // void *address_14 = memory_allocation(65536);
    // memory_free(address_13);
    // memory_free(address_12);
    // memory_free(address_14);
    // void *address_15 = memory_allocation(256);
    // memory_free(address_15);

    // void *address_1 = memory_allocation(64);
    // void *address_2 = memory_allocation(64);
    // void *address_3 = memory_allocation(64);
    // void *address_4 = memory_allocation(64);
    // void *address_5 = memory_allocation(64);
    // memory_free(address_5);
    // memory_free(address_1);
    // memory_free(address_2);
    // memory_free(address_4);
    // memory_free(address_3);

    return 0;
}