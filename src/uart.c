#include "uart.h"
#include "mmio.h"
#include "string.h"

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

void uart_sendi (int num) {
    char buffer[256];
    int isNegative = 0;
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    for (int i = 0; i < 255; i++) {
        buffer[i] = '0' + num % 10;
        num /= 10;
        if (!num) {
            buffer[i + 1] = '\0';
            break;
        }
    }

    /* reverse buffer */
    int len = strlength(buffer);
    for (int i = 0; i < len / 2; i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[len - i - 1];
        buffer[len - i - 1] = tmp;
    }
    if (isNegative)
        uart_sendc('-');
    uart_send(buffer);
}

void uart_sendh (unsigned int num) {
    char buffer[256];

    for (int i = 0; i < 255; i++) {
        int tmp = num % 16;
        char c;
        if (tmp < 10)
            c = tmp + '0';
        else
            c = tmp - 10 + 'a';

        buffer[i] = c;
        num /= 16;
        if (!num) {
            buffer[i + 1] = '\0';
            break;
        }
    }

    /* reverse buffer */
    int len = strlength(buffer);
    for (int i = 0; i < len / 2; i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[len - i - 1];
        buffer[len - i - 1] = tmp;
    }
    uart_send("0x");
    uart_send(buffer);
}

void uart_sendf (float num) {
    char integer[256];
    int isNegative = 0;

    if (num < 0) {
        isNegative = 1;
        num = - num;
    }

    int tmp = (int)num;

    /* integer */
    for (int i = 0; i < 255; i++) {
        integer[i] = '0' + tmp % 10;
        tmp /= 10;
        if (!tmp) {
            integer[i + 1] = '\0';
            break;
        }
    }

    /* reverse buffer */
    int len = strlength(integer);
    for (int i = 0; i < len / 2; i++) {
        char tmp = integer[i];
        integer[i] = integer[len - i - 1];
        integer[len - i - 1] = tmp;
    }
    if (isNegative)
        uart_sendc('-');
    uart_send(integer);
    uart_sendc('.');

    num -= tmp;
    for (int i = 0; i < 3; i++) {
        num *= 10;
        unsigned char c = (int)num % 10;
        num -= c;
        uart_sendc((char)(c + '0'));
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

#define clear(r, n) *mmio(r) &= ~(1 << n)
#define set(r, n) *mmio(r) |= 1 << n

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
    *mmio(GPPUD) = 0;
    delay_cycles(150);
    /* clock the control signal into the GPIO */
    set(GPPUDCLK0, 14);
    set(GPPUDCLK0, 15);
    delay_cycles(150);
    *mmio(GPPUDCLK0) = 0;

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
    delay_cycles(150);
}

