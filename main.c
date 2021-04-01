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

#include "bmalloc.h"
#include "buddy.h"
#include "ramdisk.h"
#include "uart.h"

int strCmp(const char* p1, const char* p2)
{
    const unsigned char* s1 = (const unsigned char*)p1;
    const unsigned char* s2 = (const unsigned char*)p2;
    unsigned char c1, c2;
    do {
        asm volatile("nop");
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}

void buddy_test()
{
    buddy_init();
    uint64_t* x1 = bmalloc(1 << 12);
    uint64_t* x2 = bmalloc(1 << 13);
    uint64_t* x3 = bmalloc(1 << 14);
    uint64_t* x4 = bmalloc(1 << 5);
    uint64_t* x5 = bmalloc(1 << 5);
    uint64_t* x6 = bmalloc(34);
    bfree(x1);
    bfree(x2);
    bfree(x3);
    bfree(x4);
    bfree(x5);
    bfree(x6);
}

void main()
{
    // set up serial console
    uart_init();
    uart_puts("Booting...\n");
    char uart_buffer[50];
    char* ramdisk = (char*)0x20000000;
    while (1) {
        uart_puts("> ");
        uart_gets(uart_buffer, 50);
        if (strCmp(uart_buffer, "help") == 0) {
            uart_puts("help:  print all available commands\n");
            uart_puts("hello:  print Hello World!\n");
        } else if (strCmp(uart_buffer, "hello") == 0) {
            uart_puts("Hello World!\n");
        } else if (strCmp(uart_buffer, "reboot") == 0) {
            reset(10);
        } else if (strCmp(uart_buffer, "cancel") == 0) {
            cancel_reset();
        } else if (strCmp(uart_buffer, "ramdisk") == 0) {
            initrd_list(ramdisk);
        } else if (strCmp(uart_buffer, "buddy") == 0) {
            buddy_test();
        } else {
            uart_puts(uart_buffer);
            uart_puts("  Command not found\n");
        }
    }
}
