#include "mini_uart.h"
#include "utils.h"
#include "shell.h"
#include "str_tool.h"

#define MAX_INPUT 10

#define PM_PASSWORD 0x5a000000
#define PM_RSTC 0x3F10001c
#define PM_WDOG 0x3F100024

struct CMD {
    char *name;
    char *help;
    void (*func)(void);
};
struct CMD command[] = {
    {.name="hello", .help="print Hello World!", .func=shell_hello},
    {.name="help", .help="print all available commands", .func=shell_help},
    {.name="reboot", .help="reboot the machine", .func=shell_reboot},
};

char input_buffer[MAX_INPUT+1];
int input_tail_idx = 0;

void buffer_clear(){
    input_tail_idx = 0;
    input_buffer[input_tail_idx] = 0;
}

void init_shell(){
    buffer_clear();
    uart_puts("Welcome to my simple shell\n");
    uart_puts("ヽ(✿ﾟ▽ﾟ)ノヽ(✿ﾟ▽ﾟ)ノヽ(✿ﾟ▽ﾟ)ノヽ(✿ﾟ▽ﾟ)ノ\n");
}

void execute_command(char *input_cmd){
    int cmd_len = sizeof(command)/sizeof(struct CMD);
    int had_executed=0;
    for(int cmd_idx=0; cmd_idx<cmd_len; cmd_idx+=1){
        if(!strcmp(input_cmd, command[cmd_idx].name)){
            had_executed = 1;
            command[cmd_idx].func();
            break;
        }
    }
    if(!had_executed){
        uart_puts("ERROR! Command not found!\n");
    }
}

void print_input_prompt(){
    uart_puts(">> ");
}
void get_input(){
    char cur_char;
    while(1){
        cur_char = uart_getc();
        uart_putc(cur_char);
        if(cur_char == '\n')
            break;
        else{
            if(input_tail_idx == MAX_INPUT){
                uart_puts("Input string meet command max limit! Please press enter or shrink the command!\n");
                continue;
            }
            input_buffer[input_tail_idx] = cur_char;
            input_tail_idx += 1;
        }
    }
    input_buffer[input_tail_idx] = 0;
}

void simple_shell(){
    init_shell();
    while(1){
        print_input_prompt();
        get_input();
        execute_command(input_buffer);
        buffer_clear();
    }
}

void shell_hello(){
    uart_puts("Hello World d(`･∀･)b\n");
}

void shell_help(){
    uart_puts("===============================================");
    uart_puts("\n");
    uart_puts("Command Name");
    uart_puts("\t");
    uart_puts("Description");
    uart_puts("\n");
    uart_puts("===============================================");
    uart_puts("\n");

    int cmd_len = sizeof(command)/sizeof(struct CMD);
    for(int cmd_idx=0; cmd_idx<cmd_len; cmd_idx+=1){
        uart_puts(command[cmd_idx].name);
        uart_puts("\t\t");
        uart_puts(command[cmd_idx].help);
        uart_puts("\n");
    }
    uart_puts("===============================================");
    uart_puts("\n");
}

void shell_reboot(){
    uart_puts("Reboot after 10 watchdog tick!\n");
    put32(PM_RSTC, PM_PASSWORD | 0x20);
    put32(PM_WDOG, PM_PASSWORD | 10);
    uart_puts("\nPree 'c' to cancel reboot\n");
    while(uart_getc()!='c');
    put32(PM_RSTC, PM_PASSWORD | 0);
    put32(PM_WDOG, PM_PASSWORD | 0);
    uart_puts("Reboot had canceled!\n");
}