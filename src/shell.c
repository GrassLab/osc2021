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
    enable_uart_interrupt();
    core_timer_enable();
    enable_irq();
    uart_send_string("=== Welcome ===\r\n");
    uart_send_string("$ ");
    while(1){
        if(cmd_flag){
            system_command(CMD_BUFFER);
            cmd_flag = 0;
            uart_send_string("$ ");
        }
    }
    // shell_welcome();
    // while(uart_get() == '\0');
    // char *buf_ptr = input_buffer;
    // __memset(buf_ptr, '\0', INPUT_SIZE);
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