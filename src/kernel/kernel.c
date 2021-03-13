#include "mini_uart.h"
#include "command.h"
#include "string.h"

void kernel()
{
    // init_uart(); // init in bootloader
    puts("Welcome!\r\n");
    
    char command[MAX_COMMAND_SIZE];
    while (1)
    {
        puts("# ");
        get(command, MAX_COMMAND_SIZE);
        exec_command(command);
    }
}