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
#include "string.h"

void main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("Wait for read kernel img...\n");

    while(uart_getc() != 's');

    uart_puts("Start to read kernel size...\n");

    int kernel_size = 0;
    for (int i = 0; i < 4; ++i)
    {
        char c = uart_mygetc();

        kernel_size |= c << (i * 8);
    }

    uart_puts("KernelSize: ");
    
    char temp[10];
    itoa(kernel_size, temp, 0);
    uart_puts(temp);
    uart_puts("\n");

    char *start_address = (char *)0x80000;
    for (int i = 0; i < kernel_size; ++i)
    {
	    char c = uart_mygetc();

	    *(start_address + i) = c;
    }

    goto *start_address;
}
