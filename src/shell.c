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
    uart_puts("Welcome to my simple shell\r\n");
    uart_puts("~~~~~~~~~~~~~~~~~~~~~~~\r\n");
    buffer_clear();
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
        uart_puts("ERROR! Command not found!\r\n");
    }
}

void print_input_prompt(){
    uart_puts(">> ");
}
void get_input(){
    char cur_char;
    while(1){
        cur_char = uart_getc();
        if(cur_char == '\n'){
            uart_puts("\r\n");
            break;
        }
        else if(cur_char == '\e'){
            if(uart_getc() == '['){
                cur_char = uart_getc();
                switch (cur_char)
                {
                case 'D':
                    left_key();
                    break;
                case 'C':
                    right_key();
                    break;
                case 'A':
                    up_key();
                    break;
                case 'B':
                    down_key();
                    break;
                default:
                    uart_puts("Not known\r\n");
                    break;
                }
            }
        }
        else if(cur_char == 127){
            del_key();
        }
        else{
            if(input_tail_idx == MAX_INPUT){
                uart_puts("\r\nInput string exceeds command max limit!\r\n");
                break;
            }
            uart_putc(cur_char);
            input_buffer[input_tail_idx] = cur_char;
            input_tail_idx += 1;
        }
    }
    input_buffer[input_tail_idx] = 0;
}

void del_key(){
    if(input_tail_idx>0){
        input_tail_idx -= 1;
        uart_puts("\033[1D");
        uart_puts(" ");
        uart_puts("\033[1D");
    }
}

void left_key(){
    
}
void right_key(){

}
void up_key(){

}
void down_key(){

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
    uart_puts("Hello World d(`･∀･)b\r\n");
}

void shell_help(){
    uart_puts("===============================================");
    uart_puts("\r\nCommand Name\tDescriptionr\n");
    uart_puts("===============================================");
    uart_puts("\r\n");

    int cmd_len = sizeof(command)/sizeof(struct CMD);
    for(int cmd_idx=0; cmd_idx<cmd_len; cmd_idx+=1){
        uart_puts(command[cmd_idx].name);
        uart_puts("\t\t");
        uart_puts(command[cmd_idx].help);
        uart_puts("\r\n");
    }
    uart_puts("===============================================");
    uart_puts("\r\n");
}

void shell_reboot(){
    uart_puts("Reboot after 10 watchdog tick!\r\n");
    delay(1);
    put32(PM_RSTC, PM_PASSWORD | 0x20);
    put32(PM_WDOG, PM_PASSWORD | 10);
    while(1);
}