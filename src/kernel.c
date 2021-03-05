#include "mini_uart.h"
#include "mem.h"
#include "command.h"
#include "string.h"

void kernel()
{
    init_uart();
    puts("Welcome!\r\n");
    
    char command[10];
    while (1)
    {
        fetch(command, 10);
        exec_command(command);
    }
}