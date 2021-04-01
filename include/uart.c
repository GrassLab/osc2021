#include "uart.h"
#include "mmio.h"
#include "string.h"
#include "args.h"

#define print_buffer_size 0x100
long find_special_token (char *b) {
    for (long i = 0; b[i]; i++) {
        if (b[i] == '\n') {
            return i;
        }

        if (b[i] == '%') {
            if (b[i + 1] == 'd')
                return i;
            else if (b[i + 1] == 'f')
                return i;
            else if (b[i + 1] == 'u')
                return i;
            else if (b[i + 1] == 'x')
                return i;
            else if (b[i + 1] == 's')
                return i;
            else if (b[i + 1] == 'c')
                return i;
            else if (b[i + 1] == 'p')
                return i;
        }
    }
    return -1;
}

void print (char *format, ...) {
    char buffer[print_buffer_size];
    u64 counter = 0;

    for (char *ptr = format; *ptr; ptr++) {
        if (*ptr == '%') {
            if (ptr[1] == 'd')
                counter++;
            else if (ptr[1] == 'f')
                counter++;
            else if (ptr[1] == 'u')
                counter++;
            else if (ptr[1] == 'x')
                counter++;
            else if (ptr[1] == 's')
                counter++;
            else if (ptr[1] == 'c')
                counter++;
            else if (ptr[1] == 'p')
                counter++;
        }
    }

   u64 size = strlength(format);
   va_list ap;
   va_start(ap, format);
   for (u64 i = 0; i < size;) {
        long s_size = 0;
        s_size = find_special_token(&format[i]);

        if (!s_size) {
            counter--;
            if (format[i] == '\n') {
                uart_send("\r\n");
                i += 1;
            }
            else if (format[i] == '%') {
                if (format[i + 1] == 'd') {
                    long tmp = va_arg(ap, long);
                    uart_sendi(tmp);
                }
                else if (format[i + 1] == 'f') {
                    double tmp = va_arg(ap, double);
                    uart_sendf(tmp);
                }
                else if (format[i + 1] == 'u') {
                    u64 tmp = va_arg(ap, u64);
                    uart_sendi(tmp);
                }
                else if (format[i + 1] == 'x') {
                    u64 tmp = va_arg(ap, u64);
                    uart_sendh(tmp);
                }
                else if (format[i + 1] == 's') {
                    char *tmp = va_arg(ap, char *);
                    uart_send(tmp);
                }
                else if (format[i + 1] == 'c') {
                    uart_send("[d]");
                }
                else if (format[i + 1] == 'p') {
                    uart_send("[d]");
                }
                i += 2;
            }
            else {
                uart_send("Error: print format error\r\n");
            }
        }
        else if (s_size < print_buffer_size && s_size >= 0) {
            strncopy(buffer, &format[i], s_size + 1);
            uart_send(buffer);
            i += s_size;
        }
        else {
            strncopy(buffer, &format[i], print_buffer_size);
            uart_send(buffer);
            i += print_buffer_size - 1;
        }
   }
   va_end(ap);
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
    int len = strlength(buffer);
    for (int i = 0; i < len / 2; i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[len - i - 1];
        buffer[len - i - 1] = tmp;
    }

    uart_send("0x");
    /*
    for (int i = 0; i < 16 - hexn; i++)
        uart_send("0");
    */
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

#define clear(r, n) *mmio(r) &= ~(1 << n)
#define set(r, n) *mmio(r) |= 1 << n

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
#endif
}

