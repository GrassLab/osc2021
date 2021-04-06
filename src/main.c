#include "loader.h"
#include "uart.h"
#include "string.h"
#include "reboot.h"
#include "time.h"
#include "mailbox.h"
#include "debug.h"
#include "cpio.h"
#include "fdt.h"
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
        print("Hello World!\n");
    }
    else if (!strcmp(b, "help")) {
        print("hello: print Hello World!\n");
        print("help: print all available commands\n");
        print("reboot: reboot raspi\n");
        print("time: show current time from boost\n");
        print("version: show rapi version\n");
        print("vcm: get vc memory\n");
        print("x/[num]gx [address]: print value in memory\n");
        print("fdt_info: show flattened device tree information\n");
        print("show_fdt: show all flattened device tree nodes\n");
        print("fdt [node]: search [node] information\n");
        print("ls\n");
        print("cat [file]\n");

        print("bs_bucket\n");
        print("malloc_bins\n");
        print("bs_free\n");
        print("bs_malloc\n");
        print("m_free\n");
        print("m_malloc\n");
    }
    else if (!strcmp(b, "reboot")) {
        print("reboot~~\n");
        reset(1000);
    }
    else if (!strcmp(b, "time")) {
        print("%f (s)\r\n", get_time());
    }
    else if (!strcmp(b, "version")) {
        print("board version: %x\n", (int)get_board_revision());
    }
    else if (!strcmp(b, "vcm")) {
        unsigned int base, size;
        if (get_vc_memory(&base, &size)) {
            print("base: %x\n", base);
            print("size: %x\n", size);
        }
        else
            print("fail\n");
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
    }
    else if (!strcmp(b, "malloc_bins")) {
        show_malloc_bins();
    }
    else if (!strcmp(b, "bs_bucket")) {
        buddy_system_show_buckets();
    }
    else if (!strcmp(token, "bs_malloc")) {
        bs_malloc_interface(b);
    }
    else if (!strcmp(token, "bs_free")) {
        bs_free_interface(b);
    }
    else if (!strcmp(token, "m_malloc")) {
        m_malloc_interface(b);
    }
    else if (!strcmp(token, "m_free")) {
        m_free_interface(b);
    }
    else if (mem_print(b)) {
    }
    else {
        print("No such command.\n");
    }
}

int main () {
    uart_init();
    startup_allocator_init();
    cpio_init();
    fdt_init();
    startup_lock_memory(0, 0x80000);

    buddy_system_init();
    dynamic_allocator_init();

    char buffer[BUFFER_SIZE];

    print("\n");
    print("+========================+\n");
    print("|       osdi shell       |\n");
    print("+========================+\n");


    while (1) {
        print("$ ");
        uart_getline(buffer, BUFFER_SIZE);
        parse_command(buffer);
        print("\n");
    }
}
