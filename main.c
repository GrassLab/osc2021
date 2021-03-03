#include "uart.h"
#include "system.h"

int compare(const char *a, const char *b){
    if(!(*a | *b)) return 0;
    return (*a != *b)? *a - *b : compare(++a, ++b);

}
void start_message(){
    uart_puts("------------------------\r\n");
    uart_puts("= Welcome to Raspi3 B+ =\r\n");
    uart_puts("= Author: Yuan-Yu Wu   =\r\n");
    uart_puts("------------------------\r\n");
 
}
void show_commands(){
    uart_puts("[Command] | [Description]\r\n");
    uart_puts(" help     |  print all available commands\r\n");
    uart_puts(" hello    |  print Hello World\r\n");
    uart_puts(" reboot   |  reboot raspi3\r\n");
}
void recieve_cmd(char *str){
    for(int i = 0; i < 1024; ++i) str[i] = '\0';
    char ch;
    for(int i = 0; ch = uart_get(); ++i){
        if(ch == '\r'){
            uart_puts("\r\n");
            break;
        }
        str[i] = ch;
        uart_send(ch);
    }
}
void shell(){
    start_message();
    
    char str[1024], c; 
    int idx = 0;
    while(1){
        uart_puts("$ ");
        recieve_cmd(str);
        // c = uart_get();
        // if(c == '\r') uart_send('\n');
        // uart_send(c);
        // str[idx++] = c;
        
        // if(c == '\r'){
        //     str[idx - 1] = '\0';
            if(compare(str, "hello") == 0) uart_puts("Hello World\n\r");
            else if(compare(str, "help") == 0) show_commands();
            else if(compare(str, "reboot") == 0){
                // uart_puts("MDFK\r\n");
                break;
            }
        //     idx = 0;
        //     uart_puts("$ ");
        // }

    }
    reset(100);
}
void main(void){
    uart_init();
    while(uart_get() == '\0');
    shell();    
}
