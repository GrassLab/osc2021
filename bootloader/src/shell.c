#include "uart.h"
#include "utils.h"

#define CMD_LEN 128

/* Reboot */
#define PM_PASSWORD    0x5a000000
#define PM_RSTC        ((volatile unsigned int*)(0x3F10001c))
#define PM_WDOG        ((volatile unsigned int*)(0x3F100024))

/* Load img */
#define KERNEL_ADDR     0x100000

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

static void load_img() {
    print("Start loading kernel image\n");
    print("Please send kernel image from UART now\n");
    unsigned int kernel_size = (int)uart_getc();
    for (int i = 1; i < 4; i++) {
        char c = uart_getc();
        kernel_size *= 256;
        kernel_size += (int)c;
    }
    unsigned long long checksum = (int)uart_getc();
    for (int i = 1; i < 5; i++) {
        char c = uart_getc();
        checksum *= 256;
        checksum += (int)c;
    }

    print("kernel size: ");
    print_int(kernel_size);
    print("\nLoading...");
    char *kernel = (char*)KERNEL_ADDR;
    for (int i = 0; i < kernel_size; i++) {
        char c = uart_getc();
        *(kernel + i) = c;
        checksum -= (int)c;
    }
    if (checksum) {
        print("\nData error\n");
    }
    else {
        print("\nDone\n\n");
        print("Start OS...\n");
        void (*start_os)(void) = (void*)kernel;
        start_os();
    }
}

static void cmd_controler(const char *cmd) {
    /* Eliminate space at the front */
    while (*cmd == (char)32) {
        cmd++;
    }

    if (!strcmp(cmd, "help")) {
        print("help\n");
        print("loadimg\n");
        print("reboot\n");
    }
    else if (!strcmp(cmd, "loadimg")) {
        load_img();
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
    while (c != '\r') {
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
    /* Flush first character */
    uart_getc();
    /* Print enter information */
    print("==========================================\n");
    print("=== Raspberry Pi 3 Model b+ Bootloader ===\n");
    print("==========================================\n");

    while (1) {
        print("% ");
        get_cmd(cmd);
        cmd_controler(cmd);
    }
}
