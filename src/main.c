#include "shell.h"
#include "uart.h"
#include "printf.h"
#include "memory.h"

int main()
{
    uart_init();
    init_printf(0, putc);
    printf("Hello World!\n\n");

    init_buddy();
    struct page *block_1 = block_allocation(4);
    block_free(block_1);

    return 0;
}