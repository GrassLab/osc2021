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

#include "uart.h"
#include "gpio.h"

/* Auxilary mini UART registers */
#define AUX_ENABLE ((volatile unsigned int*)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR ((volatile unsigned int*)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR ((volatile unsigned int*)(MMIO_BASE + 0x00215054))
#define AUX_MU_MSR ((volatile unsigned int*)(MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH ((volatile unsigned int*)(MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_STAT ((volatile unsigned int*)(MMIO_BASE + 0x00215064))
#define AUX_MU_BAUD ((volatile unsigned int*)(MMIO_BASE + 0x00215068))

#define PM_RSTC ((volatile unsigned int*)(MMIO_BASE + 0x0010001c))
#define PM_WDOG ((volatile unsigned int*)(MMIO_BASE + 0x00100024))
#define PM_PASSWORD 0x5a000000

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    *AUX_ENABLE |= 1; // enable UART1, AUX mini uart
    *AUX_MU_CNTL = 0; // turn off Tx, Rx
    *AUX_MU_LCR = 3; // 8 bits
    *AUX_MU_MCR = 0; // turn off auto flow control
    *AUX_MU_IER = 0; // disable interrupt
    *AUX_MU_IIR = 0xc6; // disable interrupts
    *AUX_MU_BAUD = 270; // 115200 baud
    /* map UART1 to GPIO pins */
    r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // gpio14, gpio15
    r |= (2 << 12) | (2 << 15); // alt5
    *GPFSEL1 = r;
    *GPPUD = 0; // enable pins 14 and 15
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0; // flush GPIO setup
    *AUX_MU_CNTL = 3; // enable Tx, Rx
}

/**
 * Send a character
 */
void uart_send(unsigned int c)
{
    /* wait until we can send */
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x20));
    /* write the character to the buffer */
    *AUX_MU_IO = c;
}

/**
 * Receive a character
 */
char uart_getc()
{
    char r;
    /* wait until something is in the buffer */
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x01));
    /* read it and return */
    r = (char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}

/**
 * Display a string
 */
void uart_puts(char* s)
{
    while (*s) {
        /* convert newline to carrige return + newline */
        if (*s == '\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

void uart_Wputs(char* s, int width)
{
    while (width--) {
        uart_send(*s++);
    }
    uart_send('\r');
    uart_send('\n');
}

void buffer_clean(char* buffer, int length)
{
    for (int i = 0; i < length; i++) {
        buffer[i] = '\0';
    }
}

void uart_gets(char* buffer, int length)
{
    buffer_clean(buffer, length);
    int ptr = 0;
    while (ptr < length) {
        buffer[ptr] = uart_getc();
        uart_puts(&buffer[ptr]);
        if (buffer[ptr] == '\n') {
            buffer[ptr] = '\0';
            break;
        }
        ptr += 1;
    }
    if (ptr == length) {
        buffer[ptr - 1] = '\0';
    }
}

void reset(int tick)
{ // reboot after watchdog timer expire
    *PM_RSTC = PM_PASSWORD | 0x20; // full reset
    *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
}

void cancel_reset()
{
    *PM_RSTC = PM_PASSWORD | 0; // full reset
    *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}

/**
 * Display a binary value in hexadecimal
 */
void uart_hex(unsigned int d)
{
    unsigned int n;
    int c;
    for (c = 28; c >= 0; c -= 4) {
        // get highest tetrad
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9 ? 0x37 : 0x30;
        uart_send(n);
    }
}

void print_string(char* s)
{
    uart_puts(s);
}

void print_char(char c)
{
    uart_send(c);
}

void print_integer(uint64_t i)
{
    if (i < 0) {
        print_char('-');
        i = -i;
    }
    if (i >= 10)
        print_integer(i / 10);
    print_char(i % 10 + '0');
}

void print_address(uint64_t addr)
{
    uint64_t quotient, remainder;
    uint64_t divisor = 0x1000000000000000;
    remainder = addr;
    print_string("0x");
    for (int i = 0; i < 16; i++) {
        quotient = remainder / divisor;
        remainder %= divisor;
        divisor /= 16;
        if (quotient < 10) {
            print_integer((int)quotient);
        } else if (quotient == 10) {
            print_char('A');
        } else if (quotient == 11) {
            print_char('B');
        } else if (quotient == 12) {
            print_char('C');
        } else if (quotient == 13) {
            print_char('D');
        } else if (quotient == 14) {
            print_char('E');
        } else if (quotient == 15) {
            print_char('F');
        } else {
            print_char('Z');
        }
    }
}