#include "mini_uart.h"
#include "command.h"
#include "string.h"

void kernel()
{
    init_uart();
    puts("Welcome!\r\n");
    
    char command[MAX_COMMAND_SIZE];
    while (1)
    {
        fetch(command, MAX_COMMAND_SIZE);
        exec_command(command);
    }
}