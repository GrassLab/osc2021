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
#include "vfs.h"
#include "sdhost.h"
#include "fat32.h"

void foo () {
    for (int i = 0; i < 10; i++) {
        printf("thread id: %d %d\n", get_pid(), i);
        sleep(1);
    }
    exit(0);
}

void user_test () {
    char *argv[] = {"argv_test", "-o", "arg2", 0};
    exec("argv_test", argv);
    exit(0);
}


void initd1 () {
    int N = 5;
    for(int i = 0; i < N; ++i) {
        thread(foo);
    }

    while (1) {
        int pid = wait();
        if (pid)
            printf("release process %d\n", pid);
    }
}

void initd2 () {
    thread(user_test);

    while (1) {
        int pid = wait();
        if (pid)
            printf("release process %d\n", pid);
    }
}

void initd_lab6 () {
    char buf[256];
    int a = open("hello", O_CREAT);
    int b = open("world", O_CREAT);
    write(a, "Hello ", 6);
    write(b, "World!", 6);
    close(a);
    close(b);
    b = open("hello", O_READ);
    a = open("world", O_READ);
    int sz;
    sz = read(b, buf, 100);
    sz += read(a, buf + sz, 100);
    buf[sz] = '\0';
    kprintf("%s\n", buf); // should be Hello World!
    while (1);
}

void initd_lab7 () {
    char buf[256];
    int a = open("fat32/hello.txt", O_CREAT);
    int b = open("fat32/world.txt", O_CREAT);
    write(a, "Hello ", 6);
    write(b, "World!", 6);
    close(a);
    close(b);
    b = open("fat32/hello.txt", O_READ);
    a = open("fat32/world.txt", O_READ);
    int sz;
    sz = read(b, buf, 100);
    sz += read(a, buf + sz, 100);
    buf[sz] = '\0';
    kprintf("%s\n", buf); // should be Hello World!
    while (1);
}

void create_initd (void (* init_func)(void)) {
    struct task_struct *t = init_task_struct();
    t->kstack_top = bs_malloc(STACK_SIZE);
    t->stack_top = bs_malloc(STACK_SIZE);
    current_task = t;
    set_sp((u64)(t->kstack_top + STACK_SIZE));
    set_sp_el0((u64)(t->stack_top + STACK_SIZE));
    from_el1_to_el0(init_func);
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
    else if (!strcmp(token, "ls")) {
        vfs_ls(&b[i + 1]);
        //cpio_show_files();
    }
    else if (!strcmp(token, "mkdir")) {
        //kprintf("%s\n", &b[i + 1]);
        vfs_mkdir(&b[i + 1]);
    }
    else if (!strcmp(token, "touch")) {
        vfs_touch(&b[i + 1]);
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
    else if (!strcmp(b, "lab5-demo1")) {
        create_initd(initd1);
    }
    else if (!strcmp(b, "lab5-demo2")) {
        create_initd(initd2);
    }
    else if (!strcmp(b, "lab6-demo")) {
        create_initd(initd_lab6);
    }

    else if (!strcmp(b, "lab7-demo")) {
        create_initd(initd_lab7);
    }

    else if (!strcmp(b, "test")) {
        //create_initd(initd_lab6);
        struct file *f = vfs_open("fat32/t1.txt", O_CREAT);
        char buffer[0x800];
        memset(buffer, 'a', 0x200);
        memset(&buffer[0x200], 'b', 0x200);
        vfs_write(f, buffer, 0x400);
        //vfs_write(f, buffer, 0x400);
        //struct file *f = vfs_open("fat32/fixup.dat", O_READ);
        f = vfs_open("fat32/t1.txt", O_CREAT);
        memset(buffer, '\0', 0x800);
        u32 size = vfs_read(f, buffer, 0x800);
        kprintf("%s\n", buffer);
        kprintf("%x\n", size);
        kprintf("%x\n", strlen(buffer));
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

    init_vfs();

    sd_init();
    fat32_init();

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
