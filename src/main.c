#include "shell.h"
#include "uart.h"
#include "printf.h"
#include "memory.h"

int main()
{
    uart_init();
    init_printf(0, putc);
    printf("Hello World!\n\n");

    init_memory();

    // struct page *block_1 = block_allocation(4);
    // struct page *block_2 = block_allocation(7);
    // block_free(block_1);
    // struct page *block_3 = block_allocation(2);
    // block_free(block_2);
    // struct page *block_4 = block_allocation(0);
    // struct page *block_5 = block_allocation(9);
    // block_free(block_3);
    // block_free(block_4);
    // block_free(block_5);
    // block_free(block_1);
    void *address_1 = memory_allocation(8192);
    void *address_2 = memory_allocation(4096);
    memory_free(address_1);
    void *address_3 = memory_allocation(16384);
    memory_free(address_2);
    memory_free(address_3);

    return 0;
}