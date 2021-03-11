#include "gpio.h"
#include "aux.h"
#include "utils.h"

void uart_init() {
    register unsigned int r = *GPFSEL1;

    *AUX_ENABLE |= 1;   /* Enable mini uart */
    *AUX_MU_CNTL = 0;   /* Disable tx rx */
    *AUX_MU_LCR = 3;    /* 8-bit mode */
    *AUX_MU_MCR = 0;
    *AUX_MU_IER = 0;    /* Disable interrupts */
    *AUX_MU_IIR = 6;    /* No fifo */
    *AUX_MU_BAUD = 270; /*Set baud rate */

    /* Change GPIO 14 15 to alternate function -> P92 */
    r &= ~((7 << 12)|(7 << 15)); /* Reset GPIO 14, 15 */
    r |= (2 << 12)|(2 << 15);    /* Set ALT5 */
    *GPFSEL1 = r;

    /* Disable GPIO pull up/down -> P101 */
    *GPPUD = 0; /* Set control signal to disable */
    delay(150);
    /* Clock the control signal into the GPIO pads */
    *GPPUDCLK0 = (1 << 14)|(1 << 15);
    delay(150);
    *GPPUDCLK0 = 0; /* Remove the clock */
    *AUX_MU_CNTL = 3; /* Enable tx rx */
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
