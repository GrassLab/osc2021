#include "command.h"
#include "mini_uart.h"
#include "str.h"
#include "peripheral.h"
#include "ops.h"

void exec_command(char *input)
{
    if (strcmp(input, "help") == 0)
    {
        send_string("1. help\r\n");
        send_string("2. hello\r\n");
    } else if (strcmp(input, "hello") == 0) 
    {
        send_string("Hello World!\r\n");
    } else if (strcmp(input, "reboot") == 0) {
        send_string("rebooting...\r\n");
        reboot(100);
    } else {
        send_string("Try another command\r\n");
    }
}

void reboot(int tick)
{
    put32(PM_RSTC, PM_PASSWORD | 0x20); // full reset
    put32(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick

    while(1);
}