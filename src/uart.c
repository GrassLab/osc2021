#include "uart.h"
#include "queue.h"
#include "gpio.h"
#include "aux.h"
#include "utils.h"
#include "config.h"
#include "sched.h"

#define BUFF_SIZE 128

char read_buff[BUFF_SIZE];
char write_buff[BUFF_SIZE];
queue_t read_queue;
queue_t write_queue;

io_t *uart_in = &read_buff;
io_t *uart_out = &write_buff;

void uart_init() {
    register unsigned int r = *GPFSEL1;

    *AUX_ENABLE |= 1;   /* Enable mini uart */
    *AUX_MU_CNTL = 0;   /* Disable tx rx */
    *AUX_MU_LCR = 3;    /* 8-bit mode */
    *AUX_MU_MCR = 0;
    *AUX_MU_IIR = 6;    /* No fifo */
    *AUX_MU_BAUD = 270; /*Set baud rate */

    /* Set interrupts */
    *AUX_MU_IER = 0;
    #ifdef UART_INTERRUPT_ENABLE
        *IRQ_S1 |= (1 << 29);
    #endif

    /* Initial read, write buffer */
    queue_init(&read_queue, read_buff, BUFF_SIZE);
    queue_init(&write_queue, write_buff, BUFF_SIZE);

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

size_t async_write(const char *s, size_t size) {
    if (!size)
        return 0;
    size_t count = 0;
    for(count; count < size; count++) {
        if (buffer_push(*(s + count), uart_out) < 0)
            break;
        if (*(s + count) == '\n')
            buffer_push('\r', uart_out);
    }
    /* Enable tx interrupt */
    *AUX_MU_IER |= 0x2;
    return count;
}

size_t async_read(char *s, size_t size) {
    if (!size)
        return 0;
    /* Enable rx interrupt */
    *AUX_MU_IER |= 0x1;
    thread_t *current = get_current();
    current->state = wait;
    current->read_size = size;
    schedule();

    char c;
    unsigned int count = 0;
    for (count; count < size; count++) {
        if (buffer_empty(uart_in) == true)
            break;
        c = buffer_pop(uart_in);
        *(s + count) = c;
    }
    return count;
}

int buffer_push(char c, io_t *io) {
    if (queue_push(io) >= 0) {
        *((char*)(io->buffer + io->back -1)) = c;
        return 0;
    }
    return -1;
}

char buffer_pop(io_t *io) {
    if (queue_pop(io) >= 0)
        return *((char*)(io->buffer + io->front - 1));
    return (char)0;
}

void buffer_flush(io_t *io) {
    while (queue_pop(io) >= 0) {}
}

bool_t buffer_empty(io_t *io) {
    return queue_empty(io);
}

bool_t buffer_full(io_t *io) {
    return queue_full(io);
}

void buffer_lock(io_t *io) {
    queue_lock(io);
}

void buffer_unlock(io_t *io) {
    queue_unlock(io);
}

//=======================================
void print(const char *s) {
    while (*s) {
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}

void print_int(unsigned long long num) {
    if (!num) {
        uart_putc('0');
        return ;
    }
    int buffer[25];
    int count = 0;
    while (num) {
        buffer[count++] = num % 10;
        num /= 10;
    }
    for (int i = count-1; i >= 0; i--)
        uart_putc((char)(buffer[i] + 48));
}

static void print_hex(unsigned long long num) {
    if (!num) {
        uart_putc('0');
        return ;
    }
    int buffer[25];
    int count = 0;
    while (num) {
        buffer[count++] = num % 16;
        num /= 16;
    }
    for (int i = count-1; i >= 0; i--)
        if (buffer[i] < 10) {
            uart_putc((char)(buffer[i] + 48));
        } else {
            uart_putc((char)(buffer[i] + 55));
        }
}

void print_sysreg(unsigned long long spsr,
                  unsigned long long elr,
                  unsigned long long esr) {
    print("Spsr: ");
    print_hex(spsr);
    print("\n");
    print("Elr: ");
    print_hex(elr);
    print("\n");
    print("Esr: ");
    print_hex(esr);
    print("\n\n");
}

void print_timer(unsigned long long cntpct,
                 unsigned long long cntfrq) {
    print("Time: ");
    print_int(cntpct / cntfrq);
    print(" seconds\n\n");
}
