#include "uart.h"


void uart_init()
{
    register unsigned int reg;

    /* initialize UART */
    *AUX_ENABLE     |= 1;       /* enable mini UART */
    *AUX_MU_CNTL     = 0;       /* Disable transmitter and receiver during configuration. */
    *AUX_MU_IER      = 0;       /* Disable interrupt */
    *AUX_MU_LCR      = 3;       /* Set the data size to 8 bit. */
    *AUX_MU_MCR      = 0;       /* Don’t need auto flow control. */
    *AUX_MU_BAUD     = 270;     /* 115200 baud */
    *AUX_MU_IIR      = 6;       /* No FIFO */
    

    /* map UART1 to GPIO pins */
    reg = *GPFSEL1;
    reg &= ~((7<<12)|(7<<15));  /* address of gpio 14, 15 */
    reg |=   (2<<12)|(2<<15);   /* set to alt5 */
    *GPFSEL1 = reg;             /* enable gpio 14 and 15 */

    *GPPUD = 0;                 /*  disable pull-up/down */
	/*  disable need time */
    reg=150;
    while ( reg-- )
    { 
        asm volatile("nop"); 
    }
    
    *GPPUDCLK0 = (1<<14)|(1<<15);
	/*  disable need time */
    reg=150; 
    while ( reg-- )
    {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0;             /* flush GPIO setup */

    *AUX_MU_CNTL = 3;           /* Enable the transmitter and receiver. */
}


void uart_send(char c) {
    do {
        asm volatile("nop");
    } while( ! ( *AUX_MU_LSR&0x20 ));
    *AUX_MU_IO = c;
}

char uart_get(){
    char r;
    do{        
        asm volatile("nop");        
    } while ( ! ( *AUX_MU_LSR&0x01 ) );
    r = ( char )( *AUX_MU_IO );
    return r;
}


void uart_puts(char *s){
    while(*s){
    	if(*s == '\n') uart_send('\r');
	uart_send(*s++);
    }
}

int uart_get_int(){
    int res = 0;
    char c;
    while(1){
        c = uart_get();
        if(c == '\0' || c == '\n')
            break;
        uart_send(c);
        res = res * 10 + (c - '0');
    }
    return res;
}




