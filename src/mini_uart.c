#include "utils.h"
#include "peripherals/gpio.h"
#include "peripherals/mini_uart.h"

void uart_init(){
    unsigned int selector;

    selector = get32(GPFSEL1);
    selector &= ~(7<<12);       // clean gpio 14
    selector |= 2<<12;          // set alt5 at gpio14
    selector &= ~(7<<15);       // clean gpio 15
    selector |= 2<<15;          // set alt5 at gpio 15
    put32(GPFSEL1, selector);

    put32(GPPUD, 0);            // disable pull-up/down
    delay(150);                 // wait 150 cycles
    put32(GPPUDCLK0, (1<<14)|(1<<15));  // only modify GPIO 14 and 15
    delay(150);
    put32(GPPUDCLK0, 0);        // remove the clock

    put32(AUX_ENABLES, 1);      // enable mini UART
    put32(AUX_MU_CNTL_REG, 0);  // Disable transmitter and receiver during configuration
    put32(AUX_MU_IER_REG, 0);   // Disable interrupt
    put32(AUX_MU_LCR_REG, 3);   // Set the data size to 8 bit
    put32(AUX_MU_MCR_REG, 0);   // Don’t need auto flow control
    put32(AUX_MU_BAUD_REG, 270);    // Set baud rate to 115200

    put32(AUX_MU_IIR_REG, 6);   // No FIFO
    put32(AUX_MU_CNTL_REG, 3);  // Enable the transmitter and receiver
}

void uart_putc(char c){
    while(!(get32(AUX_MU_LSR_REG)&0x20));
    put32(AUX_MU_IO_REG, c);
}

char uart_getc(void){
    while(!(get32(AUX_MU_LSR_REG)&0x01));
    return get32(AUX_MU_IO_REG);
}

void uart_puts(char* str){
    while(*str){
        uart_putc(*str);
        str += 1;
    }
}