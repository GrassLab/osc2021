#include "gpio.h"
#include "aux.h"
#include "utils.h"

void uart_init() {
    register unsigned int r = *GPFSEL1;

    *AUX_ENABLE |= 1;
    *AUX_MU_CNTL = 0;
    *AUX_MU_LCR = 3;
    *AUX_MU_MCR = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_IIR = 6;
    *AUX_MU_BAUD = 270;

    r &= ~((7 << 12)|(7 << 15));
    r |= (2 << 12)|(2 << 15);
    *GPFSEL1 = r;
    *GPPUD = 0;
    delay(150);
    *GPPUDCLK0 = (1 << 14)|(1 << 15);
    delay(150);
    *GPPUDCLK0 = 0;
    *AUX_MU_CNTL = 3;
}

char uart_getc() {
    char c;
    while (!(*AUX_MU_LSR & 0x01)) {}
    c = (char)(*AUX_MU_IO);
    return c == '\r'?'\n':c;
}

void uart_putc(char c) {
    while (!(*AUX_MU_LSR & 0x20)) {}
    *AUX_MU_IO = c;
}

void print(const char *s) {
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');
        }
        uart_putc(*s++);
    }
}
