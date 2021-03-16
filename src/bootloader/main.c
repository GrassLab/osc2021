#include "mini_uart.h"
#include "utils.h"
#include "stdint.h"

union large_int_by_byte{
    uint8_t int_by_byte[8];
    uint64_t larg_int;
};

void boot_main(){
    uart_init();

    union large_int_by_byte program;
    volatile uint8_t *program_addr = (volatile uint8_t *)0x80000;

    while(1) {
        uint8_t data;
        data = uart_getc();
        if (data != 1) continue;
label_1:
        data = uart_getc();
        if (data == 1) goto label_1;
        if (data != 2) continue;
        
        data = uart_getc();
        if (data == 1) goto label_1;
        if (data != 3) continue;
        
        data = uart_getc();
        if (data == 1) goto label_1;
        if (data != 4) continue;
        break;
    }

    for(int i=0; i<8; i++)
        program.int_by_byte[i] = uart_getc();

    for(uint64_t offset=0; offset<program.larg_int; offset++)
        program_addr[offset] = uart_getc();

    asm volatile("br %x[program_addr]"::[program_addr]"r"(program_addr));
}