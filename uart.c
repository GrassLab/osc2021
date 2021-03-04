#include "uart.h"
#include <stdio.h>

void delay_cycles (unsigned int num) {
    for (int i = 0; i < num; i++) {
        asm volatile("nop");
    }
}

void uart_sendc (char c) {
    while (!(*aux(MU_LSR) & 0x20));
    *aux(MU_IO) = c;
}

void uart_send (char *str) {
    while (*str) {
        uart_sendc(*str);
        ++str;
    }
}

char uart_getc () {
    char c;
    while (!(*aux(MU_LSR) & 0x1));
    c = (unsigned char) *aux(MU_IO);
    return c;
}

void uart_getline (char *buffer, unsigned int size) {
    for (int i = 0; i < size; i++)
        buffer[i] = 0;

    for (int i = 0; i < size - 1; i++) {
        char c = uart_getc();
        if (c == '\n' || c == '\r') {
            uart_send("\r\n");
            break;
        }
        uart_sendc(c);
        buffer[i] = c;
    }
}

#define clear(r, n) *regp(r) &= ~(1 << n)
#define set(r, n) *regp(r) |= 1 << n

void uart_init () {
    /* GPIO 15 takes alternate function 5 */
    clear(GPFSEL1, 17);
    set(GPFSEL1, 16);
    clear(GPFSEL1, 15);

    /* GPIO 14 takes alternate function 5 */
    clear(GPFSEL1, 14);
    set(GPFSEL1, 13);
    clear(GPFSEL1, 12);

    /* turn off GPIO pull-up/down */
    *regp(GPPUD) = 0;
    delay_cycles(150);
    /* clock the control signal into the GPIO */
    set(GPPUDCLK0, 14);
    set(GPPUDCLK0, 15);
    delay_cycles(150);
    *regp(GPPUDCLK0) = 0;

    /* Enable mini UART */
    *aux(ENABLES) |= 1;
    /* Disable transmitter and receiver */
    *aux(MU_CNTL) = 0;
    /* Disable interrupt */
    *aux(MU_IER) = 0;
    /* Set the data size to 8 bit */
    *aux(MU_LCR) = 3;
    /* Don’t need auto flow control */
    *aux(MU_MCR) = 0;
    /* Set baud rate to 115200 */
    *aux(MU_BAUD) = 270;
    /* No FIFO */
    *aux(MU_IIR) = 6;
    /* Enable the transmitter and receiver */
    *aux(MU_CNTL) = 3;
}

