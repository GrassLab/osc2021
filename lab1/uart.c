#include "gpio.h"
#include "auxilary.h"

void uart_init()
{
    
    *AUX_ENABLE |=1;
    *AUX_MU_CNTL = 0;
	*AUX_MU_IER = 0;
    *AUX_MU_LCR = 3;
    *AUX_MU_MCR = 0;
    *AUX_MU_BAUD = 270;
    *AUX_MU_IIR = 6;
	
    register unsigned int r = *GPFSEL1;
    r&=~((7<<12)|(7<<15));
    r|=(2<<12)|(2<<15);
    *GPFSEL1 = r;
	
    *GPPUD = 0;
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;
	
    *AUX_MU_CNTL = 3;

}

void uart_sendchar(unsigned int c) 
{
    do
	{
		asm volatile("nop");
	}while(!(*AUX_MU_LSR&0x20));
    
    *AUX_MU_IO = c;
}

char uart_getchar() 
{
    char r;
	
    do
	{
		asm volatile("nop");
	}while(!(*AUX_MU_LSR&0x01));
    
    r = (char)(*AUX_MU_IO);
    return (r == '\r') ? '\n' : r;
}

void uart_putstr(char *s) 
{
    while(*s) {
        if(*s=='\n')
            uart_sendchar('\r');
        uart_sendchar(*s++);
    }
}
