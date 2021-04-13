#include "mini_uart.h"
#include "command.h"
#include "io.h"
#include "pf_alloc.h"

#include "cpio.h"

void kernel()
{
    init_uart();
    init_page_frame();

    printf("***********************************\r\n");
    printf("Welcome!\r\n");
    printf("***********************************\r\n");
    
    char command[MAX_COMMAND_SIZE];
    while (1)
    {
        printf("# ");
        get(command, MAX_COMMAND_SIZE);
        exec_command(command);
    }
}


void print_num(long int a, long int b, long int c)
{
    printf("spsr_el1: %b, elr_el1: %b, esr_el1 %b\n", a, b, c);
}