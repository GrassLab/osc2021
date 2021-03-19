#include "command.h"
#include "mini_uart.h"
#include "string.h"
#include "peripheral.h"
#include "base_ops.h"
#include "cpio.h"

void exec_command(char *input)
{
    if (strcmp(input, "help") == 0) {
        puts("1. help\r\n");
        puts("2. hello\r\n");
        puts("3. reboot\r\n");
        puts("4. open\r\n");
    } else if (strcmp(input, "hello") == 0) {
        puts("Hello World!\r\n");
    } else if (strcmp(input, "reboot") == 0) {
        puts("rebooting...\r\n");
        reboot(0);
    } else if (strcmp(input, "open") == 0) {
        puts("enter file name: ");
        char filename[MAX_COMMAND_SIZE];
        get(filename, MAX_COMMAND_SIZE);
        cpio_read(filename);
        
    } else {
        puts("Try another command\r\n");
    }
}

void get(char *command, int maxSize)
{
    char c;
    int i = 0;
    memset(command, 0, sizeof(char) * maxSize);

    while(1) {
        // TODO: max command length limitation
        c = getchar();
        putchar(c);
        
        switch (c) {
            case '\r':
                putchar('\n');
                break;
            case '\b':
                puts(" \b");
                i--;
                i = i < 0 ? 0 : i;
                break;
            default:
                command[i] = c;
                i++;
        }

        if (c == '\r') {
            break;
        }
    }
}

void reboot(int tick)
{
    put32(PM_RSTC, PM_PASSWORD | 0x20); // full reset
    put32(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick

    while(1);
}