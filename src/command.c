#include "mini_uart.h"
#include "str.h"

void exec_command(char *input)
{
    if (strcmp(input, "help") == 0)
    {
        send_string("1. help\n");
        send_string("2. hello\n");
    } else if (strcmp(input, "hello") == 0) 
    {
        send_string("Hello World!\n");
    } else {
        send_string("Try another command\n");
    }
}