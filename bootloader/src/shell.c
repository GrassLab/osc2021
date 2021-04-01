#include "shell.h"
#include "string.h"
#include "uart.h"
#include "system.h"



void shell_welcome(){
    uart_puts("\r\n------------------------\r\n");
    uart_puts("=      Bootloader      =\r\n");
    uart_puts("------------------------\r\n");
}


void shell() {
    while(uart_get() == '\0');
	while(uart_get() == '\0');
    shell_welcome();

    uart_puts(USER_NAME);
    uart_puts("@");
    uart_puts(MACHINE_NAME);
	uart_puts("$ ");

    char input_buffer[MAX_BUFFER_LEN];
    char *buf_ptr = input_buffer;
    memset (buf_ptr, '\0', MAX_BUFFER_LEN);   
    char c;
    while(1){
        c = uart_get();
        if(c == '\r'){
            uart_puts("\r\n");
            system_command(input_buffer);
            
			uart_puts(USER_NAME);
			uart_puts("@");
			uart_puts(MACHINE_NAME);
			uart_puts("$ ");
            buf_ptr = input_buffer;
            memset(buf_ptr, '\0', MAX_BUFFER_LEN);
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

