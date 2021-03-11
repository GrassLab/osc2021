#include "uart.h"
#include "reboot.h"

#define MAXCMDSIZE 32

char cmdBuffer[MAXCMDSIZE];
int cmdSize = 0;

void clear_buffer() {
    cmdSize = 0;
    for (int i = 0; i < MAXCMDSIZE; i++) {
        cmdBuffer[i] = 0;
    }
    return;
}

int strlen(char* s) {
    int length = 0;
    while (s[length] != '\0') {
        length++;
    }

    return length;
}

int strcmp(char* s1, char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);

    if (len1 != len2) {
        return 0;
    }

    for (int i = 0; i < len1; i++) {
        if (s1[i] != s2[i]) {
            return 0;
        }
    }
    return 1;
}

void cmdhandler() {
    if (strcmp(cmdBuffer, "help")) {
        uart_putchar("\n>> Available Commands as follow\n");
        uart_putchar("    >> help: print all available commands\n");
        uart_putchar("    >> hello: print string Hello World!\n");
    } else if (strcmp(cmdBuffer, "hello")) {
        uart_putchar("\n>> Hello World!\n");
    } else if (strcmp(cmdBuffer, "reboot")) {
        uart_putchar("\n>>Start rebooting...\n");
        reset(100);
    } else {
        uart_putchar("\n>> Unsupported command, ");
        uart_putchar("Command input: ");
        uart_putchar(cmdBuffer);
        uart_putchar("\n");
    }
    return;
}


int main() {
    uart_init();
    clear_buffer();
    uart_putchar(">> Simple Shell starts\n");
    char c;
    while (1) {
        c = uart_read();
        uart_write(c);
        switch (c)
        {
        case '\n':
            cmdBuffer[cmdSize] = '\0';
            cmdhandler();
            clear_buffer();
            break;
        
        default:
            if (c > 96 && c < 123) {  // a to z
                cmdBuffer[cmdSize++] = c;
            }
        }
    }
    return 0;
}