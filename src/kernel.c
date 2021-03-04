#include "mini_uart.h"
#include "mem.h"
#include "command.h"

void kernel()
{
    init_uart();
    
    send_string("Welcome to PiShell!\r\n");

    char command[10];
    int commandIdx = 0;
    clearmembyte((unsigned long)command, 10);

    char c;
    while (1)
    {
        c = receive_char();
        send_char(c);
        if (c == '\r') {
            send_char('\n');
            exec_command(command);
            clearmembyte((unsigned long)command, 10);
            commandIdx = 0;
        } else {
            command[commandIdx] = c;
            commandIdx++;
        }
        // send_char(c);
    }
}