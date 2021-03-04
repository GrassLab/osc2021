#include "gpio.h"

#define AUX_MU_IO_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR_REG ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215054))
#define AUX_MU_MSR_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH ((volatile unsigned int*)(MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_STAT_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215064))
#define AUX_MU_BAUD ((volatile unsigned int*)(MMIO_BASE + 0x00215068))
#define AUXENB ((volatile unsigned int*)(MMIO_BASE + 0x00215004))

void uart_init(){
    register unsigned int reg;
    *AUXENB |= 1;
    *AUX_MU_CNTL_REG = 0;
    *AUX_MU_IER_REG = 0;
    *AUX_MU_LCR_REG = 3;
    *AUX_MU_MCR_REG = 0;
    *AUX_MU_BAUD = 270;
    *AUX_MU_IIR_REG = 0xc6;
    reg = *GPFSEL1;
    reg &= ~((7 << 12) | (7 << 15));
    reg |= (2 << 12) | (2 << 15);
    *GPFSEL1 = reg;
    *GPPUD = 0;
    reg = 150;
    while(reg--) { asm volatile("nop");}
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    reg = 150;
    while(reg--) { asm volatile("nop");}
    *GPPUDCLK0 = 0;
    *AUX_MU_CNTL_REG = 3;

}
void uart_send(unsigned int c){
    do {
    	asm volatile("nop");
    }while(~(*AUX_MU_LSR_REG)&0x20);
    *AUX_MU_IO_REG = c;
	
}
char uart_get(){
    char res;
    do {
    	asm volatile("nop");
    } while(~(*AUX_MU_LSR_REG)&0x01);
    res = (char)(*AUX_MU_IO_REG);
    // if(res == '\r') return '\n';
    return res;
}

void uart_puts(char *s){
    while(*s){
    	if(*s == '\n') uart_send('\r');
	// if(*s == '\r') uart_send('\n');
	uart_send(*s++);
    }
}
