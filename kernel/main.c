#include "../include/uart.h"
#include "../include/myshell.h"

int main(){

    uart_init(); // setup uart
    uart_puts("\n========================================\n");
    uart_puts("\t\tRaspi3\n");
    uart_puts("========================================\n");
    my_shell();

}
