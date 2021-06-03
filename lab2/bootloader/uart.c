#include "gpio.h"
#include "uart.h"
#include "string.h"
#include "util.h"

#define AUX_ENABLE  ((volatile unsigned int*)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO   ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER  ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR  ((volatile unsigned int*)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR  ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR  ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR  ((volatile unsigned int*)(MMIO_BASE + 0x00215054))
#define AUX_MU_CNTL ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_BAUD ((volatile unsigned int*)(MMIO_BASE + 0x00215068))

void uart_init() {
    *AUX_ENABLE |= 1;
    *AUX_MU_CNTL = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_LCR = 3;
    *AUX_MU_MCR = 0;
    *AUX_MU_BAUD = 270;
    *AUX_MU_IIR = 6;
    *GPFSEL1 &= ~((7 << 12) | (7 << 15));
    *GPFSEL1 |= (2 << 12) | (2 << 15);
    
    *GPPUD = 0;
    for(int i = 0; i < 150; i++) asm volatile("nop");
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    for(int i = 0; i < 150; i++) asm volatile("nop");
    *GPPUDCLK0 = 0;
    
    *AUX_MU_CNTL = 3;
    uart_get_char();
}

char uart_get_char() {
    char c;
    while ( !(*AUX_MU_LSR&0x01) );
    c = (char)(*AUX_MU_IO);
    return c;
}

int uart_get_int() {
    unsigned int num;
    num = uart_get_char()<<24;
    num |= uart_get_char()<<16;
    num |= uart_get_char()<<8;
    num |= uart_get_char();
    return num;
}

void uart_send(unsigned int c) {
    if(c == 10)
        uart_send('\r');
    
    while( !(*AUX_MU_LSR&0x20) );
    *AUX_MU_IO = c;
}

void uart_put_str(char *s) {
    while(*s)
        uart_send(*s++);
}

void uart_put_int(int num) {
    if(num/10)
		uart_put_int(num/10);
    uart_send((num%10) + '0');
}

void uart_put_addr(unsigned long addr)
{
    uart_put_str("0x");
    for(int i = 15;i >= 0; i--)
    {
        int num = (addr >> (i * 4)) & 0xf;
        num += '0';
        if(num > '9')
            num += 'a' - ('9' + 1);
        uart_send(num);
    }
}