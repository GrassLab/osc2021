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

void exec_command(char *input)
{
    if (strcmp(input, "help") == 0) {
        char *support_cmds[] = {"help", "hello", "reboot", "open", "demo1", "demo2", "load"};
        for (int i = 1; i <= 7; i++) {
            printf("%d. %s\r\n", i, support_cmds[i]);
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
        // load app
        cpio_exec("app");

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