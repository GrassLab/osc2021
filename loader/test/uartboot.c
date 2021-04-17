#include "utils.h"
#include "uart.h"
#define printf uart_puts
void uart_int(unsigned long s){
    char tmp_string[20];
    dec_hex(s,tmp_string);
    uart_puts(tmp_string);
}

void loadimg() {

    volatile unsigned char *prog;
    unsigned char code;
    unsigned int size;
    unsigned int addr;

    printf("Waiting for a program to be sent from the UART ...\n");
    addr = uart_getc_boot();
    addr = addr + (uart_getc_boot() << 8);
    addr = addr + (uart_getc_boot() << 16);
    addr = addr + (uart_getc_boot() << 24);
    prog = (unsigned char *) addr;
    size = uart_getc_boot();
    size = size + (uart_getc_boot() << 8);
    size = size + (uart_getc_boot() << 16);
    size = size + (uart_getc_boot() << 24);
    
    for (int idx = 0; idx < size; idx++)
    {
        code = uart_getc_boot();
        prog[idx] = code;
    }
    
    printf("raspi3 executes code at ");
    uart_int(addr);
    printf(", size = ");
    uart_int(size);
    printf(" bytes.\n");
    printf("-----------------------------------------------------------");
    printf("------------\n");

    void (*jump_to_new_kernel)(void) = (void (*)())addr;
    jump_to_new_kernel();
}