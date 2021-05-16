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
#include "sched.h"
#include "lib.h"

void foo () {
    for (int i = 0; i < 10; i++) {
        kprintf("thread id: %d %d\n", get_pid(), i);
        sleep(1);
    }
    while(1);
}

void initd () {
    for (int i = 0; i < 10; i++) {
        thread(foo);
    }

    while(1);
}

void create_initd () {
    struct task_struct *t = task_queue_pop_head(&unready_queue);
    t->kstack_top = bs_malloc(STACK_SIZE);
    t->stack_top = bs_malloc(STACK_SIZE);
    current_task = t;
    set_sp((u64)(t->kstack_top + STACK_SIZE));
    set_sp_el0((u64)(t->stack_top + STACK_SIZE));
    from_el1_to_el0(initd);
}

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
        //init_sched();
        //cpio_load_file_interface("./user-process");
        create_initd();
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
    enable_DAIF_irq();
    startup_allocator_init();
    cpio_init();
    fdt_init();
    startup_lock_memory(0, 0x80000);

    buddy_system_init();
    dynamic_allocator_init();

    char buffer[BUFFER_SIZE];

    init_sched();

    kprintf("\n");
    kprintf("+========================+\n");
    kprintf("|       osdi shell       |\n");
    kprintf("+========================+\n");

    while (1) {
        kprintf("$ ");
        read_line(buffer, BUFFER_SIZE);
        strip_newline(buffer);
        parse_command(buffer);
        kprintf("\n");
    }
}
