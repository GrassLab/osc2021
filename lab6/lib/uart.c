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

#include "../include/gpio.h"
#include "../include/uart.h"
#include "../include/printf.h"
/* Auxilary mini UART registers */

void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    *AUX_ENABLE |=1;       // enable UART1, AUX mini uart
    *AUX_MU_CNTL = 0;
    *AUX_MU_LCR = 3;       // 8 bits
    *AUX_MU_MCR = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_IIR = 0xc6;    // disable interrupts
    *AUX_MU_BAUD = 270;    // 115200 baud
    /* map UART1 to GPIO pins */
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // gpio14, gpio15
    r|=(2<<12)|(2<<15);    // alt5
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;        // flush GPIO setup
    *AUX_MU_CNTL = 3;      // enable Tx, Rx


    while(*AUX_MU_LSR&0x01){
        char c = (char)*AUX_MU_IO;
    }
}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
    /* wait until we can send */
    if(c == '\n'){
        uart_send('\r');
    }
    do{asm volatile("nop");}while(!(*AUX_MU_LSR&0x20));
    /* write the character to the buffer */
    *AUX_MU_IO=c;
}

/**
 * Receive a character
 */
char uart_getc() {
    char r;
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(!(*AUX_MU_LSR&0x01));
    /* read it and return */
    r=(char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return r=='\r'?'\n':r;
}

unsigned char uart_getb(){//for data transfer
	unsigned char r;
	do{ asm volatile("nop"); }while(!(*AUX_MU_LSR&0x01));
	r=(unsigned char)(*AUX_MU_IO);
	return r;
}

int uart_gets(char* s,int size,int display){
	for(int i=0;;++i){
		if(i==size){
			uart_puts("buffer overflow!\n");
			return i;
		}

		s[i]=uart_getc();
		if(display)uart_send(s[i]);

		if(s[i]=='\n'){
			s[i]=0;
			return i;
		}
	}
}

void uart_hex(unsigned int d) {
    unsigned int n;
    int c;
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_send(n);
    }
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
        uart_send(*s++);
    }
}

static char getHexChar(char c) {
    return (c > 9) ? c - 0xa + 'a' : c + '0';
}

void sendHexUART(unsigned int hex) {
    uart_puts("0x");
    for (int i = 1; i <= sizeof(unsigned int); ++i) {
        char c = (hex >> ((sizeof(unsigned int) - i) * 8)) & 0xFF;

        // send upper 4-bit
        uart_send(getHexChar(c >> 4));

        // send lower 4-bit
        uart_send(getHexChar(c & 0xf));
    }
}

unsigned int uart_printf(char* fmt,...){
	char dst[100];
    //__builtin_va_start(args, fmt): "..." is pointed by args
    //__builtin_va_arg(args,int): ret=(int)*args;args++;return ret;
    __builtin_va_list args;
    __builtin_va_start(args,fmt);
    unsigned int ret=vsprintf(dst,fmt,args);
    uart_puts(dst);
    return ret;
}
