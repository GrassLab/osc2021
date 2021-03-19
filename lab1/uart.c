#include "gpio.h"

#define AUX_ENABLE  ((volatile unsigned int*)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO   ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER  ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR  ((volatile unsigned int*)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR  ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR  ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR  ((volatile unsigned int*)(MMIO_BASE + 0x00215054))
#define AUX_MU_CNTL ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_BAUD ((volatile unsigned int*)(MMIO_BASE + 0x00215068))

void uart_init()
{
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
}

char uart_getc()
{
    char c;
    while ( !(*AUX_MU_LSR&0x01) );
    c = (char)(*AUX_MU_IO);
    if ( c == '\r') { c = '\n'; }
    return c;
}

void uart_send(unsigned int c)
{
    while( !(*AUX_MU_LSR&0x20) );
    *AUX_MU_IO = c;
}

void uart_puts(char *s)
{
    while(*s)
    {
        if(*s == '\n')
        {
            uart_send('\r');
        }
        uart_send(*s++);
    }
}
