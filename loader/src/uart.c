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

#include "gpio.h"
#include "uart.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int reg;

    /* initialize UART */
    *AUX_ENABLE |= 1; /* enable mini UART */
    *AUX_MU_CNTL = 0; /* Disable transmitter and receiver during configuration. */

    *AUX_MU_IER = 0;    /* Disable interrupt */
    *AUX_MU_LCR = 3;    /* Set the data size to 8 bit. */
    *AUX_MU_MCR = 0;    /* Don’t need auto flow control. */
    *AUX_MU_BAUD = 270; /* 115200 baud */
    *AUX_MU_IIR = 6;    /* No FIFO */
    // *AUX_MU_IIR      = 0xc6;       /* No FIFO */

    /* map UART1 to GPIO pins */
    reg = *GPFSEL1;
    reg &= ~((7 << 12) | (7 << 15)); /* address of gpio 14, 15 */
    reg |= (2 << 12) | (2 << 15);    /* set to alt5 */

    *GPFSEL1 = reg;

    *GPPUD = 0; /* enable gpio 14 and 15 */
    reg = 150;
    while (reg--)
    {
        asm volatile("nop");
    }

    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    reg = 150;
    while (reg--)
    {
        asm volatile("nop");
    }

    *GPPUDCLK0 = 0; /* flush GPIO setup */

    *AUX_MU_CNTL = 3; // Enable the transmitter and receiver.
}

/**
 * Send a character
 */
void uart_send(unsigned int c)
{
    /* Wait until we can send */
    do
    {

        asm volatile("nop");

    } while (!(*AUX_MU_LSR & 0x20));

    /* write the character to the buffer */
    *AUX_MU_IO = c;

    if (c == '\n')
    {
        do
        {

            asm volatile("nop");

        } while (!(*AUX_MU_LSR & 0x20));

        *AUX_MU_IO = '\r';
    }
}

/**
 * Receive a character
 */
char uart_getc()
{
    char r;

    /* wait until something is in the buffer */
    do
    {

        asm volatile("nop");

    } while (!(*AUX_MU_LSR & 0x01));

    /* read it and return */
    r = (char)(*AUX_MU_IO);
    /* convert carrige return to newline 
       because pressing enter only sends \r */
    return r == '\r' ? '\n' : r;
}

/**
 * Display a string
 */
void uart_puts(char *s)
{
    while (*s)
    {
        /* convert newline to carrige return + newline */

        //if(*s=='\n')
        //    uart_send('\r');

        uart_send(*s++);
    }
}

void putc(void *p, char c)
{
    uart_send(c);
}