#include "uart.h"
#include "utils.h"

#define CMD_LEN 128

/* Reboot */
#define PM_PASSWORD    0x5a000000
#define PM_RSTC        ((volatile unsigned int*)(0x3F10001c))
#define PM_WDOG        ((volatile unsigned int*)(0x3F100024))

/* Load img */
#define KERNEL_ADDR     0x80000

/* Key */
#define LEFT_BRACKET    (char)91
#define DELETE          (char)127
#define TAB             (char)9
#define ESC             (char)27

extern char _relocate_start;
extern char _relocate_end;
extern char _bootloader_offset;

static void reset(int tick) {
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | tick;
}

static void cancel_reset() {
    *PM_RSTC = PM_PASSWORD | 0;
    *PM_WDOG = PM_PASSWORD | 0;
}

__attribute__((section(".text.bootloader"))) void bootloader(unsigned int kernel_size,
                                                             unsigned long long checksum) {
    char *kernel_addr = (char*)KERNEL_ADDR;
    for (int i = 0; i < kernel_size; i++) {
        char c = uart_getc();
        *(kernel_addr + i) = c;
        checksum -= (int)c;
    }
    if (!checksum) {
        print("Start OS\n\n");
        void (*start_os)(void) = (void*)kernel_addr;
        start_os();
    }
}

static void load_img() {
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

    print("Kernel size: ");
    print_dec(kernel_size);
    unsigned int code_size = (unsigned int)(&_relocate_end - &_relocate_start);
    char *new_addr = (char*)((kernel_size / 0x10000 + 1) * 0x10000 + KERNEL_ADDR);
    char *old_addr = (char*)&_relocate_start;
    print("\nRelocating.....\n");
    for (int i = 0; i < code_size; i++)
        *(new_addr + i) = *(old_addr + i);

    print("Loading kernel.....\n");
    void (*start_bootloader)(unsigned int, unsigned long long) = (void*)(new_addr + (unsigned int)&_bootloader_offset);
    start_bootloader(kernel_size, checksum);
}

static void cmd_controler(const char *cmd) {
    /* Eliminate space at the front */
    while (*cmd == (char)32)
        cmd++;

    if (!strcmp(cmd, "help")) {
        print("help:\n");
        print("    List all available command.\n");
        print("loadimg:\n");
        print("    Load kernel image via UART.\n");
        print("reboot:\n");
        print("    Reboot raspberry pi.\n");
    } else if (!strcmp(cmd, "loadimg")) {
        load_img();
    } else if (!strcmp(cmd, "reboot")) {
        print("Rebooting.....\n\n");
        reset(100);
        while (1) {}
    } else {
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
