#include "command.h"
#include "io.h"

void shell()
{
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

// handler
void print_num(int a, int b, int c)
{
    printf("spsr_el1: %b, elr_el1: %b, esr_el1 %b\n", a, b, c);
}
