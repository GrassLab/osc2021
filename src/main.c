#include "bootloader.h"
#include "io.h"
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
#include "utility.h"
#define BUFFER_SIZE 64

void parse_command (char *b) {
    char token[128];
    int i = 0;
    for (; i < 128 && b[i] != '\0' && b[i] != ' '; i++) {
        token[i] = b[i];
    }
    token[i] = '\0';

    if (!strcmp(token, "hello")) {
        kprintf("Hello World!\n");
    }
    else if (!strcmp(b, "help")) {
        kprintf("hello: kprintf Hello World!\n");
        kprintf("help: kprintf all available commands\n");
        kprintf("reboot: reboot raspi\n");
        kprintf("time: show current time from boost\n");
        kprintf("version: show rapi version\n");
        kprintf("vcm: get vc memory\n");
        kprintf("x/[num]gx [address]: kprintf value in memory\n");
        kprintf("fdt_info: show flattened device tree information\n");
        kprintf("show_fdt: show all flattened device tree nodes\n");
        kprintf("fdt [node]: search [node] information\n");
        kprintf("ls\n");
        kprintf("cat [file]\n");

        kprintf("bs_bucket\n");
        kprintf("malloc_bins\n");
        kprintf("bs_free\n");
        kprintf("bs_malloc\n");
        kprintf("m_free\n");
        kprintf("m_malloc\n");
    }
    else if (!strcmp(b, "reboot")) {
        kprintf("reboot~~\n");
        reset(1000);
    }
    else if (!strcmp(b, "time")) {
        kprintf("%f (s)\r\n", get_time());
    }
    else if (!strcmp(b, "version")) {
        kprintf("board version: %x\n", (int)get_board_revision());
    }
    else if (!strcmp(b, "vcm")) {
        unsigned int base, size;
        if (get_vc_memory(&base, &size)) {
            kprintf("base: %x\n", base);
            kprintf("size: %x\n", size);
        }
        else
            kprintf("fail\n");
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
        from_el1_to_el0();
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
    else if (mem_print(b)) {
    }
    else if (cpio_load_file_interface(b)) {
    }
    else {
        kprintf("No such command.\n");
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

    //from_el1_to_el0();

    char buffer[BUFFER_SIZE];

    kprintf("\n");
    kprintf("+========================+\n");
    kprintf("|       osdi shell       |\n");
    kprintf("+========================+\n");


    while (1) {
        kprintf("$ ");
        uart_getline(buffer, BUFFER_SIZE);
        parse_command(buffer);
        kprintf("\n");
    }
}
