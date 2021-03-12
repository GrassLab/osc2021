#include "uart.h"
#include "string.h"
#include "reboot.h"
#include "time.h"
#include "mailbox.h"
#include "debug.h"
#include "loader.h"
#include "utility.h"
#define BUFFER_SIZE 64

void parse_command (char *b) {
    if (!strcmp(b, "hello")) {
        uart_send("Hello World!\n");
    }
    else if (!strcmp(b, "help")) {
        uart_send("hello: print Hello World!\r\n");
        uart_send("help: print all available commands\r\n");
        uart_send("reboot: reboot raspi\r\n");
        uart_send("time: show current time from boost\r\n");
        uart_send("version: show rapi version\r\n");
        uart_send("vcm: get vc memory\r\n");
        uart_send("x/[num]gx [address]: print value in memory\r\n");
        uart_send("load: load kernel image via mini-uart\r\n");
    }
    else if (!strcmp(b, "reboot")) {
        uart_send("reboot~~\n");
        reset(10000);
    }
    else if (!strcmp(b, "time")) {
        uart_sendf(get_time());
        uart_send(" (s)\r\n");
    }
    else if (!strcmp(b, "version")) {
        uart_sendh((int)get_board_revision());
        uart_send("\r\n");
    }
    else if (!strcmp(b, "vcm")) {
        unsigned int base, size;
        if (get_vc_memory(&base, &size)) {
            uart_send("base: ");
            uart_sendh(base);
            uart_send("\r\nsize: ");
            uart_sendh(size);
            uart_send("\r\n");
        }
        else
            uart_send("fail\r\n");
    }
    else if (!strcmp(b, "load")) {
        uart_send("address: ");
        char buffer[BUFFER_SIZE];
        uart_getline(buffer, BUFFER_SIZE);
        unsigned long address = atoui(buffer);
        uart_send("size: ");
        uart_getline(buffer, BUFFER_SIZE);
        unsigned long size = atoui(buffer);

        /* send kernel base address and size */
        uart_send("+========================+\r\n");
        uart_send("|       kernel info      |\r\n");
        uart_send("+========================+\r\n");
        uart_send("address: ");
        uart_sendh(address);
        uart_send("\r\n");
        uart_send("size: ");
        uart_sendh(size);
        uart_send("\r\n");

        long checksum = remote_load((unsigned long *)address, size);
        uart_send("checksum: ");
        uart_sendh(checksum);
        uart_send("\r\n");
    }
    else if (!strcmp(b, "jump")) {
        jump_address(0x80000);
    }
    else if (!strcmp(b, "test")) {
    }
    else if (mem_print(b)) {
    }
    else {
        uart_send("No such command.\n");
    }
}

int main () {
    uart_init();

    char buffer[BUFFER_SIZE];

    uart_send("\r\n");
    uart_send("+========================+\r\n");
    uart_send("|       bootloader       |\r\n");
    uart_send("+========================+\r\n");

    while (1) {
        uart_send("$ ");
        uart_getline(buffer, BUFFER_SIZE);
        parse_command(buffer);
        uart_send("\r\n");
    }
}
