#include "shell.h"


void shell_welcome(){
    uart_puts("------------------------\r\n");
    uart_puts("=        Kernel        =\r\n");
    uart_puts("=                      =\r\n");
    uart_puts("= Author: Yuan-Yu Wu   =\r\n");
    uart_puts("------------------------\r\n");
}
void shell(){
    while(uart_get() == '\0');
    shell_welcome();
    uart_puts("$ ");
    char *buf_ptr = input_buffer;
    __memset(buf_ptr, '\0', INPUT_SIZE);
    char c;
    while(1){
        c = uart_get();
        if(c == '\r'){
            uart_puts("\r\n");
            system_command(input_buffer);
            
            uart_puts("$ ");
            buf_ptr = input_buffer;
            __memset(buf_ptr, '\0', INPUT_SIZE);
        }
        else if(c == '\b'){
            if(buf_ptr > input_buffer){
                uart_puts("\b \b");
                *(--buf_ptr) = '\0';
            }
        }
        else{
            uart_send(c);
            *buf_ptr++ = c;
        }
    }
}