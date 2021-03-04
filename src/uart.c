#include "auxiliary.h"
#include "gpio.h"

void uart_init() {
    // auxiliary setting
    *AUX_ENABLES |= 1;
    *AUX_MU_CNTL_REG = 0;
    *AUX_MU_IER_REG = 0;
    *AUX_MU_LCR_REG = 3;
    *AUX_MU_MCR_REG = 0;
    *AUX_MU_BAUD_REG = 270;
    *AUX_MU_IIR_REG = 6;

    // GPIO setting
    register unsigned int r = *GPFSEL1;
    // reset gpio 14 15
    r &= ~((7<<12)|(7<<15));
    // set alt5 to gpio 14 15
    r |= (2<<12)|(2<<15);
    *GPFSEL1 = r;

    // disable gpio pull-down pull-up
    *GPPUD = 0;
    // wait 150 cycles
    r = 150;
    while(r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r = 150;
    while(r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0;

    // enable Rx Tx
    *AUX_MU_CNTL_REG = 3;
}

void uart_putchar(unsigned int c) {
    do {
        asm volatile("nop");
    } while(!(*AUX_MU_LSR_REG & 0x20));

    *AUX_MU_IO_REG = c;
}

char uart_getchar() {
    do {
        asm volatile("nop");
    } while(!(*AUX_MU_LSR_REG & 0x01));

    char r = (char)(*AUX_MU_IO_REG);
    return r=='\r'?'\n':r;

}

void uart_putstr(char* str) {
    while(*str) {
        // \r\n
        if(*str=='\n') {
            uart_putchar('\r');
        }
        uart_putchar(*str);
        str++;
    }
}
