#include "shell.h"
#include "uart.h"
#include "gpio.h"
#include "cpio.h"
#include "string.h"
#include "allocator.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC     ((volatile unsigned int*)(MMIO_BASE + 0x0010001c))
#define PM_WDOG     ((volatile unsigned int*)(MMIO_BASE + 0x00100024))

#define COMMAND_LIST_LEN    (sizeof(command_list) / sizeof(struct Command))

struct Command {
    char *name;
    void (*func)();
    char *description;
    
} static const command_list[] = {
    {.name = "help", .func = &help, .description = "print all available commands\nUsage: help"},
    {.name = "hello", .func = &hello, .description = "print Hello World!\nUsage: hello"},
    {.name = "ls", .func = &list, .description = "list files\nUsage: ls"},
    {.name = "cat", .func = &cat, .description = "print file's content\nUsage: cat pathname"},
    {.name = "reboot", .func = &reset, .description = "reboot\nUsage: reboot"},
    {.name = "run", .func = &run, .description = "run user program in initramfs\nUsage: run pathname"},
};


void help() {
    uart_put_str("command list :\n");
    for (int i = 0; i < COMMAND_LIST_LEN; i++) {
        uart_send('[');
        uart_put_str(command_list[i].name);
        uart_send(']');
        uart_put_str(": ");
        uart_put_str(command_list[i].description);
        uart_put_str("\n");
    }
    return;
}

void hello() {
    uart_put_str("Hello World!\n");
    return;
}

void reset() {
    uart_put_str("reboot...\n");
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | 1000;
    while (1);
}

// return char *args and *command is command's name
char *split_command_args(char *command) {
    char *str;
    while(*command != ' ') {
        if (*command == '\0')
            return "";
        command++;
    }
    // ptr -> ' '
    *command = '\0';

    while (*command++ == ' ');
    if (*command == '\0')
        return "";
    str = command;
    
    while (*command++ != '\0');
    command--;
    // ptr -> '\0'
    while (*--command == ' ')
        *command = '\0';
    
    return str;
}

void do_command(char *command) {
    char *command_args = split_command_args(command);
    for (int i = 0; i < COMMAND_LIST_LEN; i++) {
        if (strcmp(command, command_list[i].name)) {
            command_list[i].func(command_args);
            return;
        }
    }

    if (strcmp(command, "")) {
        return;
    }
    else {
        uart_put_str("command not found.\n");
        return;
    }
}

void shell() {
    char *command;
    while (1) {
        uart_put_str("pi@rpi3 ~> ");
        command = uart_get_str();
        do_command(command);
    }
}