#include "uart.h"
#include "mmio.h"
#include "string.h"
#include "utility.h"
#include "io.h"

#define BUFFER_SIZE 0x1000
char read_buffer[BUFFER_SIZE], write_buffer[BUFFER_SIZE];
long read_start = 0, read_end = 0, write_start = 0, write_end = 0;

int push_read_buffer () {
    int flag = 0;
    while (*aux(MU_LSR) & 0x1) {
        read_buffer[read_end] = (unsigned char) *aux(MU_IO);
        read_end = (read_end + 1) % BUFFER_SIZE;
        /* buffer is already full. discard oldest data. */
        if (read_start == read_end) {
            read_start = (read_start + 1) % BUFFER_SIZE;
            flag = 1;
        }
    }
    return flag;
}

int read_line (char *buffer, u32 size) {
    int is_ready = 0;
    /* block until data is ready */
    while (!is_ready) {
        long tmp, counter = 0;
        for (tmp = read_start; tmp != read_end; tmp = (tmp + 1) % BUFFER_SIZE) {
            if (read_buffer[tmp] == '\n') {
                is_ready = 1;
                break;
            }
            if (++counter > size) {
                is_ready = 1;
                break;
            }
        }
    }

    /* disable irq */
    disable_DAIF_irq();
    long anchor = read_start, i = 0;
    for (; i < size - 1 && anchor != read_end; i++,
            anchor = (anchor + 1) % BUFFER_SIZE) {
        buffer[i] = read_buffer[anchor];
        if (read_buffer[anchor] == '\n') {
            anchor = (anchor + 1) % BUFFER_SIZE;
            break;
        }
    }
    read_start = anchor;
    buffer[i + 1] = '\0';
    /* enable irq */
    enable_DAIF_irq();
    return i;
}

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

void uart_sendi (long num) {
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
    int len = strlen(buffer);
    for (int i = 0; i < len / 2; i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[len - i - 1];
        buffer[len - i - 1] = tmp;
    }
    if (isNegative)
        uart_sendc('-');
    uart_send(buffer);
}

void uart_sendhf (unsigned long num) {
    char buffer[256];
    int hexn = 0;
    for (int i = 0; i < 255; i++) {
        hexn++;
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
    int len = strlen(buffer);
    for (int i = 0; i < len / 2; i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[len - i - 1];
        buffer[len - i - 1] = tmp;
    }

    uart_send("0x");
    uart_send(buffer);
}

void uart_sendh (unsigned long num) {
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
    int len = strlen(buffer);
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
    int len = strlen(integer);
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
#ifdef UART_REPLY
            uart_send("\r\n");
#endif
            break;
        }
#ifdef UART_REPLY
        uart_sendc(c);
#endif
        buffer[i] = c;
    }
}


void uart_init () {
#ifdef UART_MINI
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
    set(AUX_MU_IER, 0); /* enable receive interrupt */
    set(ENABLE_IRQS1, 29); /* disable transmit interrupt */

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
#endif
}

