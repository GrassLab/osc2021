#include "shell.h"

extern int cmd_flag;
extern char CMD_BUFFER[UART_BUFFER_SIZE];

void shell_welcome(){
    uart_puts("------------------------\r\n");
    uart_puts("=        Kernel        =\r\n");
    uart_puts("=                      =\r\n");
    uart_puts("= Author: Yuan-Yu Wu   =\r\n");
    uart_puts("------------------------\r\n");
}
void shell(){
    sync_call_uart();
    shell_welcome();
    while(uart_get() == '\0');
    
    // uart_send_string("hi\r\n");
    uart_send_string("$ ");
    // char *buf_ptr = input_buffer;
    // __memset(buf_ptr, '\0', INPUT_SIZE);
    while(1){
        if(cmd_flag){
            // uart_send_string("hi\r\n");
            // uart_puts(CMD_BUFFER);
            system_command(CMD_BUFFER);
            cmd_flag = 0;
            uart_send_string("$ ");
        }
        // uart_send_string("xx\r\n");
    }
    // char c;
    // while(1){
    //     c = uart_get();
    //     if(c == '\r'){
    //         uart_puts("\r\n");
    //         system_command(input_buffer);
            
    //         uart_puts("$ ");
    //         buf_ptr = input_buffer;
    //         __memset(buf_ptr, '\0', INPUT_SIZE);
    //     }
    //     else if(c == '\b'){
    //         if(buf_ptr > input_buffer){
    //             uart_puts("\b \b");
    //             *(--buf_ptr) = '\0';
    //         }
    //     }
    //     else{
    //         uart_send(c);
    //         *buf_ptr++ = c;
    //     }
    // }
}