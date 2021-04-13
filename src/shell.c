#include "mini_uart.h"
#include "utils.h"
#include "shell.h"
#include "str_tool.h"
#include "stdint.h"
#include "device_tree.h"
#include "allocator.h"
#include "str_tool.h"

#define MAX_INPUT 200
#define MAX_ARGS  100

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
    {.name="ls", .help="list all the file", .func=shell_ls},
    {.name="cat", .help="print the content of the file", .func=shell_cat},
    {.name="run", .help="execute the program", .func=shell_run},
    {.name="pdtinfo", .help="print Device Tree Info", .func=print_dt_info},
    {.name="parsedt", .help="parse Device Tree", .func=parse_dt},
    {.name="memory", .help="do some memory operation", .func=shell_memory},
    {.name="getEL", .help="Get current Exception Level", .func=shell_getel}
};

void shell_getel(){
    int curEL = get_el();
    uart_puts("Current Exception Level: ");
    uart_puts(itoa(curEL, 10));
    uart_puts("\r\n");
}

char input_buffer[MAX_INPUT+1];
char *input_argv;
int input_tail_idx = 0;

void buffer_clear(){
    input_tail_idx = 0;
    input_buffer[input_tail_idx] = 0;
}

void init_shell(){
    uart_puts("Welcome to my simple shell\r\n");
    uart_puts("ヽ(✿ﾟ▽ﾟ)ノヽ(✿ﾟ▽ﾟ)ノヽ(✿ﾟ▽ﾟ)ノヽ(✿ﾟ▽ﾟ)ノ\r\n");
    buffer_clear();
}

void split_cmd_and_argv(){
    int input_len = strlen(input_buffer);
    for(int i=0; i<input_len; i++){
        if(input_buffer[i] == ' '){
            input_argv = &input_buffer[i+1];
            input_buffer[i] = 0;
            break;
        }
    }
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
        if(cur_char == '\r'){
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
                uart_puts("\r\nInput string meet command max limit!\r\n");
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
        split_cmd_and_argv();
        execute_command(input_buffer);
        buffer_clear();
    }
}

void shell_hello(){
    uart_puts("Hello World d(`･∀･)b\r\n");
}

void shell_help(){
    uart_puts("===============================================");
    uart_puts("\r\n");
    uart_puts("Command Name");
    uart_puts("\t");
    uart_puts("Description");
    uart_puts("\r\n");
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
    delay(100000);
    put32(PM_RSTC, PM_PASSWORD | 0x20);
    put32(PM_WDOG, PM_PASSWORD | 10);
}

void relocate_program(unsigned char *addr_start, unsigned char *addr_end){
    unsigned char *target_addr = (unsigned char*)0x70000;
    while(addr_start != addr_end){
        *target_addr = *addr_start;
        addr_start += 1;
        target_addr += 1;
    }
}

void shell_run(){
    char *targetFileName = input_argv;

    uint64_t cur_addr = 0x8000000;
    cpio_newc_header* cpio_ptr;
    uint64_t name_size, file_size;
    char *file_name;
    char *file_content;

    while(1){
        cpio_ptr = (cpio_newc_header*)cur_addr;
        name_size = hex_to_int64(cpio_ptr->c_namesize);
        file_size = hex_to_int64(cpio_ptr->c_filesize);

        cur_addr += sizeof(cpio_newc_header);
        file_name = (char*)cur_addr;
        if(!strcmp(file_name, "TRAILER!!!"))
            break;            

        cur_addr = (uint64_t)((cur_addr + name_size + 3) & (~3));
        file_content = (char *)cur_addr;
        cur_addr = (uint64_t)((cur_addr + file_size + 3) & (~3));

        if(!strcmp(file_name, targetFileName)){
            relocate_program((unsigned char*)file_content, (unsigned char*)cur_addr);
            run_program();
            break;
        }
    }
    if(strcmp(file_name, targetFileName)){
        uart_puts("ERROR: No Such File Or Directory!\r\n");
    }
}

void shell_cat(){
    char *targetFileName = input_argv;

    uint64_t cur_addr = 0x8000000;
    cpio_newc_header* cpio_ptr;
    uint64_t name_size, file_size;
    char *file_name;
    char *file_content;

    while(1){
        cpio_ptr = (cpio_newc_header*)cur_addr;
        name_size = hex_to_int64(cpio_ptr->c_namesize);
        file_size = hex_to_int64(cpio_ptr->c_filesize);

        cur_addr += sizeof(cpio_newc_header);
        file_name = (char*)cur_addr;
        if(!strcmp(file_name, "TRAILER!!!"))
            break;            

        if(!strcmp(file_name, targetFileName)){
            file_content = file_name + name_size;
            for(uint64_t i=0; i<file_size; i++){
                if(file_content[i] == '\n')
                    uart_putc('\r');
                uart_putc(file_content[i]);
            }
            uart_puts("\r\n");
            break;
        }
        
        cur_addr = (uint64_t)((cur_addr + name_size + 3) & (~3));
        cur_addr = (uint64_t)((cur_addr + file_size + 3) & (~3));
    }
    if(strcmp(file_name, targetFileName)){
        uart_puts("ERROR: No Such File Or Directory!\r\n");
    }
}

void shell_ls(){
    uint64_t cur_addr = 0x8000000;
    cpio_newc_header* cpio_ptr;
    uint64_t name_size, file_size;
    char *file_name;

    while(1){
        cpio_ptr = (cpio_newc_header*)cur_addr;
        name_size = hex_to_int64(cpio_ptr->c_namesize);
        file_size = hex_to_int64(cpio_ptr->c_filesize);

        cur_addr += sizeof(cpio_newc_header);
        file_name = (char*)cur_addr;
        if(!strcmp(file_name, "TRAILER!!!"))
            break;            

        uart_puts(file_name);
        uart_puts("\t\t");
        uart_puts(itoa(file_size, 10));
        uart_puts(" bytes\r\n");

        cur_addr = (uint64_t)((cur_addr + name_size + 3) & (~3));
        cur_addr = (uint64_t)((cur_addr + file_size + 3) & (~3));
    }
}

extern FrameArray *frame_array;

void shell_memory(){
    char cur_char;
    uint64_t need_size, free_addr, mem;
    struct FrameListNum *cursor;
    uart_puts("\r\nWelcome to memory manipulator!");
    while(1){
        uart_puts("\r\n\r\nEnter alphabet to do memory operation\r\n");
        uart_puts("= = = = = = = = = = = = = = = = = = = = = = = =\r\n");
        uart_puts("n: new a free memory\r\n");
        uart_puts("d: free an allocated memory\r\n");
        uart_puts("l: list current memory list\r\n");
        uart_puts("x: exit memory manipulator\r\n");

        cur_char = uart_getc();
        if(cur_char == '\r'){
            uart_puts("\r\n");
        }
        else if(cur_char == 'n'){
            uart_puts("Enter the memory size you want to new (bytes)\r\n");
            buffer_clear();
            get_input();
            need_size = atoi(input_buffer, 10);
            mem = kmalloc(need_size);
            if(mem >= 0){
                uart_puts("New Memory Address: ");
                uart_puts(itoa(mem, 16));
                uart_puts("\r\n");
            }
        }
        else if(cur_char == 'd'){
            uart_puts("Enter the allocated memory address (hex)\r\n");
            buffer_clear();
            get_input();
            free_addr = hex_to_int64(input_buffer);

            free(free_addr);
        }
        else if(cur_char == 'l'){
            uint16_t i;
            for(i=0; i<20; i++){
                if(frame_array->freeList[i]){
                    uart_puts("Frame Power: ");
                    uart_puts(itoa(i, 10));
                    uart_puts("\r\n");

                    cursor = frame_array->freeList[i];
                    while(cursor){
                        uart_puts(" -> ");
                        uart_puts(itoa(cursor->index, 10));
                        cursor = cursor->next;
                    }
                    uart_puts("\r\n");
                }
            }
        }
        else if(cur_char == 'x'){
            uart_puts("Bye~\r\n");
            break;
        }
    }
}