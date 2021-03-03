#include "mini_uart.h"

void kernel()
{
    init_uart();

    send_string("Hello! please type your command\n");

    // char command[10];
    while(1)
    {
        // command = receive_string();
        
    }
}