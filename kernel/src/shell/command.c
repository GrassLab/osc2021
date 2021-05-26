#include "command.h"
#include "mini_uart.h"
#include "string.h"
#include "peripheral.h"
#include "base_ops.h"
#include "cpio.h"
#include "pf_alloc.h"
#include "def.h"
#include "dynamic_alloc.h"
#include "io.h"
#include "thread.h"
#include "system_call.h"
#include "vfs.h"


void exec_command(char *input)
{
    if (strcmp(input, "help") == 0) {
        char *support_cmds[] = {"help", "hello", "reboot", "open", "demo1", "demo2", "load"};
        for (int i = 1; i <= 7; i++) {
            printf("%d. %s\r\n", i, support_cmds[i - 1]);
        }
    } else if (strcmp(input, "hello") == 0) {
        printf("Hello World!\r\n");
    } else if (strcmp(input, "reboot") == 0) {
        printf("rebooting...\r\n");
        reboot(0);
    } else if (strcmp(input, "open") == 0) {
        printf("enter file name: ");

        char filename[MAX_COMMAND_SIZE];
        get(filename, MAX_COMMAND_SIZE);

        cpio_read(filename);
    } else if (strcmp(input, "demo1") == 0) {
        demo1();
    } else if (strcmp(input, "demo2") == 0) {
        demo2();
    } else if (strcmp(input, "load") == 0) {
        char * argv[] = {"argv_test", "-o", "arg2", 0};
        sys_exec("argv_test", argv);
        
        while ( 1 ) {
            sys_schedule();
        }
    } else if (strcmp(input, "thread") == 0) {
        for(int i = 0; i < 10; ++i) { // N should > 2
            create_thread(foo);
        }
        
        while ( 1 ) {
            sys_schedule();
        }
    } else if (strcmp(input, "lookup") == 0) {
        // struct file *hello = vfs_open("hello", O_CREAT);
        // struct file *world = vfs_open("world", O_CREAT);
        char buf[100] = { 0 };
        int readbytes;

        int fd = open("test", O_CREAT);
        readbytes = read(fd, buf, 20);
        printf("fd: %d, read: %s, %d bytes\n", fd, buf, readbytes);
        close(fd);

        int fd2 = open("test2", O_CREAT);
        memset(buf, 0, 100);
        readbytes = read(fd2, buf, 20);
        printf("fd: %d, read: %s, %d bytes\n", fd2, buf, readbytes);
        close(fd2);

    } else if (strcmp(input, "write") == 0) {
        char buf[100] = { 0 };
        int a = open("hello", O_CREAT);
        int b = open("world", O_CREAT);
        write(a, "Hello ", 6);
        write(b, "World!", 6);
        close(a);
        close(b);
        b = open("hello", 0);
        a = open("world", 0);
        int sz;
        sz = read(b, buf, 100);
        sz += read(a, buf + sz, 100);
        buf[sz] = '\0';
        printf("%s\n", buf); // should be Hello World!

    }  else if (strcmp(input, "fat32") == 0) { 
        char buf[100] = { 0 };
        int fd = open("HELLO   ", O_CREAT);
        write(fd, "VVVVVV", 6);
        close(fd);

        fd = open("HELLO   ", O_CREAT);
        int readbytes = read(fd, buf, 20);

        for (int i = 0; i < readbytes; i++) {
            putchar(buf[i]);
        }
        printf("\n");
        // printf("fd: %d, read: %s, %d bytes\n", fd, buf, readbytes);
    } else {
        printf("Try another command\r\n");
    }
}


void get(char *command, int maxSize)
{
    char c;
    int i = 0;
    memset(command, 0, sizeof(char) * maxSize);

    while(1) {
        // TODO: max command length limitation
        c = getchar();
        putchar(c);
        
        switch (c) {
            case '\r':
                printf("\n");
                break;
            case '\b':
                printf(" \b");
                i--;
                i = i < 0 ? 0 : i;
                break;
            default:
                command[i] = c;
                i++;
        }

        if (c == '\r') {
            break;
        }
    }
}

void reboot(int tick)
{
    put32(PM_RSTC, PM_PASSWORD | 0x20); // full reset
    put32(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick

    while(1);
}

void demo1()
{
    // page frame allocator
    void *addr1 = NULL;
    void *addr2 = NULL;
    // void *addr3 = NULL;
    printf("alloc page, size: 2^2\r\n");
    alloc_page(&addr1, 2);
    printf("alloc page, size: 2^3\r\n");
    alloc_page(&addr2, 3);
    // alloc_page(&addr3, 4);
    mem_stat();
    printf("free the first page\r\n");
    free_page(addr1, 2);
    mem_stat();
    printf("free the second page\r\n");
    free_page(addr2, 3);
    mem_stat();
}

void demo2()
{
    // dynamic allocator
    void *addr1 = NULL;
    void *addr2 = NULL;
    void *addr3 = NULL;
    void *addr4 = NULL;

    addr1 = malloc(10);
    printf("dynamic allocate, size: 10 bytes\r\n");
    addr2 = malloc(128);
    printf("dynamic allocate, size: 128 bytes\r\n");
    pool_stat();

    printf("free the first block\r\n");
    free(addr1);
    pool_stat();

    printf("free the second block\r\n");
    free(addr2);
    pool_stat();

    mem_stat();

    printf("----- demo augment pool -----\r\n");
    printf("allocate 4 256 bytes chunks\r\n");
    addr1 = malloc(256);
    addr2 = malloc(256);
    addr3 = malloc(256);
    addr4 = malloc(256);
    pool_stat();
    printf("\nfree the chunks\r\n");
    free(addr1);
    free(addr2);
    free(addr3);
    free(addr4);
    pool_stat();


    mem_stat();
}


void foo() {
    for(int i = 0; i < 10; ++i) {
        struct Thread *current_thread = get_current_thread();
        printf("Thread id: %d %d\n", current_thread->tid, i);
        wait(1000000);
        sys_schedule();
    }
    sys_exit();
}
