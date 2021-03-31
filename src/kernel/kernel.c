#include "mini_uart.h"
#include "command.h"
// #include "string.h"
#include "io.h"
#include "pf_alloc.h"

void kernel()
{
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