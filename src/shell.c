#include "uart.h"
#include "util.h"

#define CMD_LEN 256

#define PM_PASSWORD    0x5a000000
#define PM_RSTC        ((volatile unsigned int*)(0x3F10001c))
#define PM_WDOG        ((volatile unsigned int*)(0x3F100024))

static void reset(int tick) {
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | tick;
}

static void cancel_reset() {
    *PM_RSTC = PM_PASSWORD | 0;
    *PM_WDOG = PM_PASSWORD | 0;
}

static void cmd_controler(const char *cmd) {
    if (!strcmp(cmd, "help")) {
        print("help\n");
        print("hello\n");
        print("reboot\n");
    }
    else if (!strcmp(cmd, "hello")) {
        print("Hello world\n");
    }
    else if (!strcmp(cmd, "reboot")) {
        print("rebooting.....\n\n");
        reset(100);
        while (1) {}
    }
    else {
        if (cmd[0]) {
            print(cmd);
            print(": command not found\n");
        }
    }
}

static void print_enter_infor() {
    print("==================================\n");
    print("=== Raspberry Pi 3 Model b+ OS ===\n");
    print("==================================\n");
}

void run_shell() {
    char cmd[CMD_LEN];
    //flush first character
    uart_getc();
    print_enter_infor();
    while (1) {
        print("% ");
        get(cmd);
        cmd_controler(cmd);
    }
}
