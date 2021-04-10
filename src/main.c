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
        printf("Hello World!\n");
    }
    else if (!strcmp(b, "help")) {
        printf("hello: printf Hello World!\n");
        printf("help: printf all available commands\n");
        printf("reboot: reboot raspi\n");
        printf("time: show current time from boost\n");
        printf("version: show rapi version\n");
        printf("vcm: get vc memory\n");
        printf("x/[num]gx [address]: printf value in memory\n");
        printf("fdt_info: show flattened device tree information\n");
        printf("show_fdt: show all flattened device tree nodes\n");
        printf("fdt [node]: search [node] information\n");
        printf("ls\n");
        printf("cat [file]\n");

        printf("bs_bucket\n");
        printf("malloc_bins\n");
        printf("bs_free\n");
        printf("bs_malloc\n");
        printf("m_free\n");
        printf("m_malloc\n");
    }
    else if (!strcmp(b, "reboot")) {
        printf("reboot~~\n");
        reset(1000);
    }
    else if (!strcmp(b, "time")) {
        printf("%f (s)\r\n", get_time());
    }
    else if (!strcmp(b, "version")) {
        printf("board version: %x\n", (int)get_board_revision());
    }
    else if (!strcmp(b, "vcm")) {
        unsigned int base, size;
        if (get_vc_memory(&base, &size)) {
            printf("base: %x\n", base);
            printf("size: %x\n", size);
        }
        else
            printf("fail\n");
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
        printf("No such command.\n");
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

    printf("\n");
    printf("+========================+\n");
    printf("|       osdi shell       |\n");
    printf("+========================+\n");


    while (1) {
        printf("$ ");
        uart_getline(buffer, BUFFER_SIZE);
        parse_command(buffer);
        printf("\n");
    }
}
