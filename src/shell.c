#include "uart.h"
#include "utils.h"
#include "cpio.h"
#include "type.h"
#include "mm.h"
#include "set_int.h"

#define CMD_LEN 128

/* Reboot */
#define PM_PASSWORD    0x5a000000
#define PM_RSTC        ((volatile unsigned int*)(0x3F10001c))
#define PM_WDOG        ((volatile unsigned int*)(0x3F100024))

/* Key */
#define LEFT_BRACKET    (char)91
#define DELETE          (char)127
#define TAB             (char)9
#define ESC             (char)27

#define EXEC_ADDR      0x1000000
/* Timer */
#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int*)0x40000040)

void *addr[100];
int addr_count;
int addr_pos;

static void reset(int tick) {
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | tick;
}

static void cancel_reset() {
    *PM_RSTC = PM_PASSWORD | 0;
    *PM_WDOG = PM_PASSWORD | 0;
}

static void execute_file(const char *file_name) {
    for (int i = 0; i < file_count; i++) {
        if (!strcmp(file_list[i].file_name, file_name)) {
            if (file_list[i].executable) {
                print("Loading user program\n\n");
                char *exec_addr = (char*)EXEC_ADDR;
                for (int j = 0; j < file_list[i].file_size; j++)
                    *(exec_addr + j) = *(file_list[i].file_content + j);
                //from_el1_to_el0(EXEC_ADDR);
            } else {
                print("exec: ");
                print(file_name);
                print(": File is not executable\n");
            }
            return ;
        }
    }
    print("exec: ");
    print(file_name);
    print(": No such file or directory\n");
}

static void free_mem(const char *num) {
    int pos = atoi(num);
    if (addr[pos] != NULL) {
        kfree(addr[pos]);
        addr[pos] = NULL;
        if (pos < addr_pos)
            addr_pos = pos;
    } else {
        print("Not found\n");
    }
}

static void allocate_mem(const char *num) {
    addr[addr_pos] = kmalloc(atoi(num));
    while(addr[++addr_pos]) {}
    addr_count++;
}

static void cat(const char *file_name) {
    unsigned int count;
    for (count = 0; count < file_count; count++) {
        if (!strcmp(file_name, file_list[count].file_name)) {
            print(file_list[count].file_content);
            break;
        }
    }
    if (count == file_count) {
        print("cat: ");
        print(file_name);
        print(": No such file or directory\n");
    }
}

static void ls() {
    for (int i = file_count; i > 0; i--) {
        print(file_list[i-1].file_name);
        print("\n");
    }
}

static void cmd_controler(char *cmd) {
    char *cmd_list[10];
    /* Eliminate space */
    unsigned int count = 0;
    while (1) {
        while (*cmd == (char)32) {
            cmd++;
        }
        cmd_list[count] = cmd;
        count++;
        while (*cmd != (char)32 && *cmd != (char)0) {
            cmd++;
        }
        if(*cmd) {
            *cmd = '\0';
            cmd++;
        } else {
            break;
        }
    }

    if (!strcmp(cmd_list[0], "help")) {
        print("help\n");
        print("    List all available command.\n");
        print("hello\n");
        print("    Print Hello world.\n");
        print("ls\n");
        print("    List all available file.\n");
        print("cat\n");
        print("    Print the file content of a speceific file\n");
        print("    Usage: cat <file_name>\n");
        print("alloc\n");
        print("    Allocate memory\n");
        print("    Usage: alloc <size>\n");
        print("free\n");
        print("    Free memeory\n");
        print("reboot\n");
        print("    Reboot raspberry pi.\n");
    } else if (!strcmp(cmd_list[0], "hello")) {
        print("Hello world\n");
    } else if (!strcmp(cmd_list[0], "ls")) {
        ls();
    } else if (!strcmp(cmd_list[0], "cat")) {
        cat(cmd_list[1]);
    } else if (!strcmp(cmd_list[0], "alloc")) {
        allocate_mem(cmd_list[1]);
    } else if (!strcmp(cmd_list[0], "free")) {
        free_mem(cmd_list[1]);
    } else if (!strcmp(cmd_list[0], "exec")) {
        execute_file(cmd_list[1]);
    } else if (!strcmp(cmd_list[0], "en_timer")) {
        print("Timer interrupt enable\n");
        *CORE0_TIMER_IRQ_CTRL = 2;
    } else if (!strcmp(cmd_list[0], "dis_timer")) {
        print("Timer interrupt disable\n");
        *CORE0_TIMER_IRQ_CTRL = 0;
    } else if (!strcmp(cmd_list[0], "pm")) {
        for (int i = 0; i < addr_count; i++) {
            if (addr[i] != NULL) {
                print("Num:");
                print_int(i);
                print(" Address:");
                print_int((unsigned int)addr[i]);
                print("\n");
            }
        }
    } else if (!strcmp(cmd_list[0], "buddy_print")) {
        buddy_print();
    } else if (!strcmp(cmd_list[0], "reboot")) {
        print("Rebooting.....\n\n");
        reset(100);
        while (1) {}
    } else {
        if (cmd_list[0][0]) {
            print(cmd_list[0]);
            print(": Command not found\n");
        }
    }
}

static void get_cmd(char *s) {
    unsigned int cursor = 0;
    int esc_state = 0;
    char c = uart_getc();
    while (c != '\n') {
        if (c == DELETE) { /* Key - Delete */
            if (cursor) {
                char delete[4] = {ESC, LEFT_BRACKET, (char)68, '\0'};
                print(delete);
                print(" ");
                print(delete);
                *(s + --cursor) = '\0';
            }
            if (esc_state) {
                esc_state = 0;
            }
        } else if (c == ESC) { /* Key - Esc */
            esc_state = 1;
        } else if (c == LEFT_BRACKET && esc_state) { /* Key - Left Bracket */
            esc_state = 0;
            uart_getc();
        } else if (c != TAB) {
            uart_putc(c);
            if (cursor < CMD_LEN - 1) {
                *(s + cursor++) = c;
            }
            if (esc_state) {
                esc_state = 0;
            }
        }
        c = uart_getc();
    }
    *(s+cursor) = '\0';
    print("\n");
}

void run_shell() {
    char cmd[CMD_LEN];
    /* Print enter information */
    //async_write("==================================\n");
    //async_write("=== Raspberry Pi 3 Model b+ OS ===\n");
    //async_write("==================================\n");
    print("Start\n");

    for(int i = 0; i < 100; i++)
        addr[i] = NULL;
    addr_count = 0;
    addr_pos = 0;

    while (1) {
        //async_write("% ");
        print("% ");
        get_cmd(cmd);
        //async_read(cmd);
        cmd_controler(cmd);
    }
}
