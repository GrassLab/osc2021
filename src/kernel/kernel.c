#include "mini_uart.h"
#include "command.h"
#include "io.h"
#include "pf_alloc.h"
#include "cpio.h"
#include "timer.h"

void kernel()
{
    init_uart();
    init_page_frame();

    printf("***********************************\r\n");
    printf("Welcome!\r\n");
    printf("***********************************\r\n");
    // cpio_read("test");
    char command[MAX_COMMAND_SIZE];
    while (1)
    {
        printf("# ");
        get(command, MAX_COMMAND_SIZE);
        exec_command(command);
    }
}

// handler
void print_num(int a, int b, int c)
{
    printf("spsr_el1: %b, elr_el1: %b, esr_el1 %b\n", a, b, c);
}
