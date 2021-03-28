#include "loader.h"
#include "uart.h"
#include "string.h"
#include "reboot.h"
#include "time.h"
#include "mailbox.h"
#include "debug.h"
#include "cpio.h"
#include "flattened_devicetree.h"
#include "mm.h"
#include "data_type.h"
#define BUFFER_SIZE 64

void parse_command (char *b) {
    char token[128];
    int i = 0;
    for (; i < 128 && b[i] != '\0' && b[i] != ' '; i++) {
        token[i] = b[i];
    }
    token[i] = '\0';

    if (!strcmp(token, "hello")) {
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
        uart_send("fdt_info: show flattened device tree information\r\n");
        uart_send("show_fdt: show all flattened device tree nodes\r\n");
        uart_send("fdt [node]: search [node] information\r\n");
    }
    else if (!strcmp(b, "reboot")) {
        uart_send("reboot~~\n");
        reset(1000);
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
    else if (!strcmp(b, "ls")) {
        cpio_show_files();
    }
    else if (!strcmp(token, "cat")) {
        cpio_cat_interface(b);
    }
    else if (!strcmp(token, "fdt_info")) {
        show_fdt_info();
    }
    else if (!strcmp(b, "show_fdt")) {
        show_all_fdt();
    }
    else if (!strcmp(b, "boot_info")) {
        show_boot_info();
    }
    /* TODO: delete */
    else if (!strcmp(b, "test")) {
        show_list();
        startup_used_list_reorder();
        uart_send("\r\n");
        show_list();
    }
    else if (!strcmp(b, "test1")) {
        unsigned long tmp = (unsigned long) fdt_head;
        tmp += get_fdt_header_off_dt_struct();
        u32 *ptr = (u32 *)tmp;
        uart_sendh(tmp);
        uart_send("\r\n");
        int flag = 0;
        for (int i = 0; i < 100; i++) {
            if (flag) {
                flag = 0;
                uart_send((char *) &ptr[i]);
            }
            if (ptr[i] == FDT_PROP_BIG) {
                uart_send((char *) &ptr[i + 3]);
                uart_send(" \r\n");
            }

            uart_send("\t");
            uart_sendh((unsigned long) &ptr[i]);
            uart_send(": ");

            uart_sendh(u32_b2l(ptr[i]));
            uart_send(" \r\n");
            if (u32_b2l(ptr[i]) == 0x1)
                flag = 1;
        }
        uart_send("\r\n");
    }
    else if (mem_print(b)) {
    }
    else {
        uart_send("No such command.\n");
    }
}

int main () {
    uart_init();
    startup_allocator_init();
    cpio_init();
    fdt_init();

    char buffer[BUFFER_SIZE];

    uart_send("\r\n");
    uart_send("+========================+\r\n");
    uart_send("|       osdi shell       |\r\n");
    uart_send("+========================+\r\n");

    while (1) {
        uart_send("$ ");
        uart_getline(buffer, BUFFER_SIZE);
        parse_command(buffer);
        uart_send("\r\n");
    }
}
