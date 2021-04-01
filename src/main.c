#include "allocator.h"
#include "uart.h"
#include "mystring.h"
#include "shell.h"

#define CMDSIZE 128

void main() {
    char cmd[CMDSIZE] = { 0 };
    int cmd_idx=0;
    uart_init();
    allocator_init();
    uart_putstr("\r\n");
    uart_putstr("# ");
    while(1) {
        char c = uart_getchar();
        switch(c) {
            case '\n':
                cmd[cmd_idx] = 0;
                uart_putstr("\n");
                shell_cmd(cmd);
                uart_putstr("# ");
                memset(cmd, 0, CMDSIZE);
                cmd_idx = 0;
                break;
            case 127:
                if(cmd_idx) {
                    uart_putstr("\b \b");
                    cmd_idx--;
                }
                break;
            default:
                if(c>31 && c<127) {
                    cmd[cmd_idx] = c;
                    cmd_idx++;
                    uart_putchar(c);
                }
                break;
        }
    }
}
