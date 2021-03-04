#include "uart.h"
#include "string.h"
#define BUFFER_SIZE 64

void parse_command (char *b) {
    if (!strcmp(b, "hello")) {
        uart_send("Hello World!\n");
    }
    else if (!strcmp(b, "reboot")) {
        uart_send("reboot\n");
    }
    else if (!strcmp(b, "help")) {
        uart_send("hello help reboot\n");
    }
    else {
        uart_send("No such command.\n");
    }
}

int main () {
    uart_init();
    char buffer[BUFFER_SIZE];

    uart_send("osdi shell\n");
    while (1) {
        uart_send("$ ");
        uart_getline(buffer, BUFFER_SIZE);
        parse_command(buffer);

        uart_send("\r\n");
    }
}
