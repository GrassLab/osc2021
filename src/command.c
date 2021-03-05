#include "command.h"
#include "mini_uart.h"
#include "string.h"
#include "peripheral.h"
#include "ops.h"

void exec_command(char *input)
{
    if (strcmp(input, "help") == 0) {
        puts("1. help\r\n");
        puts("2. hello\r\n");
    } else if (strcmp(input, "hello") == 0) {
        puts("Hello World!\r\n");
    } else if (strcmp(input, "reboot") == 0) {
        puts("rebooting...\r\n");
        reboot(100);
    } else {
        puts("Try another command\r\n");
    }
}

void fetch(char *command, int maxSize)
{
    char c;
    int i = 0;
    memset(command, 0, sizeof(char) * maxSize);

    while(1) {
        c = getchar();
        putchar(c);

        if (c == '\r') {
            putchar('\n');
            break;
        } else {
            command[i] = c;
            i++;
        }
    }
}

void reboot(int tick)
{
    put32(PM_RSTC, PM_PASSWORD | 0x20); // full reset
    put32(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick

    while(1);
}