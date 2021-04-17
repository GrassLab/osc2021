/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */
#include "shell.h"
#include "gpio.h"
#include "uart.h"
#include "base.h"
#include "exception.h"
#include "printf.h"

int uart_read_idx, uart_transmit_idx;
char UART_READ_BUFFER[MAX_BUFFER_LEN], UART_TRANSMIT_BUFFER[MAX_BUFFER_LEN];
/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
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
    // *AUX_MU_IIR      = 0xc6;       /* No FIFO */

    /* map UART1 to GPIO pins */
    reg = *GPFSEL1;
    reg &= ~((7<<12)|(7<<15));  /* address of gpio 14, 15 */
    reg |=   (2<<12)|(2<<15);   /* set to alt5 */

    *GPFSEL1 = reg;

    *GPPUD = 0;                 /* enable gpio 14 and 15 */
    reg=150;
    while ( reg-- )
    { 
        asm volatile("nop"); 
    }
    
    *GPPUDCLK0 = (1<<14)|(1<<15);
    reg=150; 
    while ( reg-- )
    {
        asm volatile("nop");
    }
    
    *GPPUDCLK0 = 0;             /* flush GPIO setup */

    *AUX_MU_CNTL = 3;           // Enable the transmitter and receiver.

    uart_read_idx = 0;
    uart_transmit_idx = 0;
}

void enable_uart_interrupt()
{
    /** Initalize and enable uart read interrupt.
     *
     * And we should not enable transmit interrupt initally becuase transmit is empty at the beginning.
     * trainsmit interrupt will be enable later if there are characters in read buffer.
     */
    *AUX_MU_IER = 0x0; 
    enable_uart_read_interrupt(); 
    // Enable second level interrupt controller’s IRQs1(0x3f00b210)’s bit29. 
    *ENABLE_IRQS_1 = AUX_IRQ; 
    printf("[enable_uart_interrupt]\n");
}

void enable_uart_read_interrupt()
{
    unsigned long int temp = *AUX_MU_IER;
    *AUX_MU_IER = temp | 0x1; // disable read interrupt(remain original uart transmit setting)
}

void enable_uart_transmit_interrupt()
{
    unsigned long int temp = *AUX_MU_IER;
    *AUX_MU_IER = temp | 0x2; // disable read interrupt(remain original uart read setting)
}
void disable_uart_read_interrupt()
{
    unsigned long int temp = *AUX_MU_IER;
    *AUX_MU_IER = temp & 0x2; // disable read interrupt(remain original uart transmit setting)
}

void disable_uart_transmit_interrupt()
{
    unsigned long int temp = *AUX_MU_IER;
    *AUX_MU_IER = temp & 0x1; // disable transmit interrupt(remain original uart read setting)
}

/**
 * mini UART read/write interrupt handler
 * 
 * Basic uart asynchronous read/write idea is that we use "read buffer" and "transmit buffer".
 *
 *  
 * Read buffer:
 * Read buffer is used to save the chars from keyboard until buffer is read by shell in shell.c.
 * If a character in read buffer is read by shell, this char will be remove from read buffer.
 * 
 * Transmit buffer:
 * It's used to print all the chars to screen until buffer is empty.
 * When shell read a character from read buffer, this character will be pushed to the tail end of transmit buffer.
 * And then chars in transmit are waiting to be pushedon screen.
 * 
 * This function need to cooperate with shell code in shell.c to achieve asynchronous r/w.
 */
void uart_irq_handler()
{
    unsigned int id = *AUX_MU_IIR;
    if((id & 0x06) == 0x04) // miniReceiver holds valid byte
    {
        // read one char and pushed to the tail end of read buffer
        char input_char = uart_getc();
        UART_READ_BUFFER[uart_read_idx++] = input_char;
    }
    else if((id & 0x06) == 0x02) // miniUART Transmit holding register empty
    {
        // print one char
        uart_send(UART_TRANSMIT_BUFFER[uart_transmit_idx - 1]);
        uart_transmit_idx--;

        // All chars in transmit buffer have been processing, then disable transmit interrupt
        if (uart_transmit_idx == 0)
            disable_uart_transmit_interrupt();
    }
} 

/**
 * Send a character
 */
void uart_send(unsigned int c)
{
    /* Wait until we can send */
    // (AUX_MU_LSR) Bit five, if set to 1, tells us that the transmitter is empty, meaning that we can write to the UART.
    do {
        
        asm volatile("nop");

    } while( ! ( *AUX_MU_LSR&0x20 ));
    
    /* write the character to the buffer */   
    *AUX_MU_IO = c;

    
    // 1. '\n' -> Newline
    // 2. '\r' -> move the cursor at the start of the line.
    if ( c == '\n' ) 
    {
        do {
            
            asm volatile("nop");

        } while( ! ( *AUX_MU_LSR&0x20 ));
        
        *AUX_MU_IO = '\r';
    }
}

/**
 * Receive a character
 */
char uart_getc() {

    char r;
    
    /* wait until something is in the buffer */
    // (AUX_MU_LSR) Bit zero, if set to 1, indicates that the data is ready
    do{
        
        asm volatile("nop");
        
    } while ( ! ( *AUX_MU_LSR&0x01 ) );

    /* read it and return */
    r = ( char )( *AUX_MU_IO );

    /* convert carrige return to newline */
    // Some editor(like screen editor) detect 'Enter' cliked keyboard will send '\r'
    return r == '\r' ? '\n' : r;
}

/**
 * Display a string
 */
void uart_puts(char *s)
{
    while( *s )
    {
        /* convert newline to carrige return + newline */
    
        //if(*s=='\n')
        //    uart_send('\r');

        uart_send(*s++);

    }
}

// This function is required by printf function
void putc ( void* p, char c)
{
	uart_send(c);
}