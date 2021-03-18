#include "uart.h"
#include "utils.h"
#include "cpio.h"

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

static void reset(int tick) {
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | tick;
}

static void cancel_reset() {
    *PM_RSTC = PM_PASSWORD | 0;
    *PM_WDOG = PM_PASSWORD | 0;
}

static void ls() {
    for (int i = 0; i < file_count; i++) {
        print("File name: ");
        print(file_list[i].file_name);
        print("\n");
        print("File content: ");
        print(file_list[i].file_content);
    }
}

static void cmd_controler(const char *cmd) {
    /* Eliminate space at the front */
    while (*cmd == (char)32) {
        cmd++;
    }

    if (!strcmp(cmd, "help")) {
        print("help\n");
        print("hello\n");
        print("ls\n");
        print("reboot\n");
    }
    else if (!strcmp(cmd, "hello")) {
        print("Hello world\n");
    }
    else if (!strcmp(cmd, "ls")) {
        ls();
    }
    else if (!strcmp(cmd, "reboot")) {
        print("Rebooting.....\n\n");
        reset(100);
        while (1) {}
    }
    else {
        if (cmd[0]) {
            print(cmd);
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
        }
        else if (c == ESC) { /* Key - Esc */
            esc_state = 1;
        }
        else if (c == LEFT_BRACKET && esc_state) { /* Key - Left Bracket */
            esc_state = 0;
            uart_getc();
        }
        else if (c != TAB) {
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
    print("==================================\n");
    print("=== Raspberry Pi 3 Model b+ OS ===\n");
    print("==================================\n");

    while (1) {
        print("% ");
        get_cmd(cmd);
        cmd_controler(cmd);
    }
}
