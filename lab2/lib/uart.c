#include "include/reg.h"

#define DATA_READY_BIT (1 << 0)
#define TRANSMITTER_EMPTY_BIT (1 << 5)
#define AUX_INT_ENABLE_BIT (1 << 29)
#define TRANSMIT_INT_ENABLE_BIT (1 << 1)
#define RECEIVE_INT_ENABLE_BIT (1 << 0)

/* terminal send \x0d instead of \x0a */
#define NEWLINE '\r'

void mini_uart_init() {
    int r;
    *GPFSEL1 &= 0xfffc0fff;
    *GPFSEL1 |= 0x00012000;

    *GPPUD = 0;
    r = 150; while(r--);
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150; while(r--);
    *GPPUDCLK0 = 0;

    *AUXENB = 1;
    *AUX_MU_CNTL_REG = 0;
    *AUX_MU_IER_REG = 0;
    *AUX_MU_LCR_REG = 3;
    *AUX_MU_MCR_REG = 0;
    *AUX_MU_BAUD = 270;
    *AUX_MU_IIR_REG = 6;
    *AUX_MU_CNTL_REG = 3;

    /* enable transmit/receive interrupt */
    //*AUX_MU_IER_REG = RECEIVE_INT_ENABLE_BIT | TRANSMIT_INT_ENABLE_BIT;
    //*INPUT_ENABLE_REGISTER_1 = AUX_INT_ENABLE_BIT;
}

void __wfe() {
    //asm("wfe");
}

/* TODO: wfe not work ? */
void read_uart(char *buffer, int len) {
    for (int i = 0; i < len; i++) {
        while (!(*AUX_MU_LSR_REG & DATA_READY_BIT)) __wfe();
        buffer[i] = *AUX_MU_IO_REG & 0xff;
    }
}

int readline_uart(char *buffer) {
    char c = '\0';
    int count = 0;
    while (c != NEWLINE) {
        while (!(*AUX_MU_LSR_REG & DATA_READY_BIT)) __wfe();
        c = *AUX_MU_IO_REG & 0xff;
        buffer[count++] = c;
    }

    buffer[count-1] = '\0';
    return count;
}

void write_uart(const char *buffer, int count) {
    for (int i = 0; i < count; i++) {
        while (!(*AUX_MU_LSR_REG & TRANSMITTER_EMPTY_BIT)) __wfe();
        *(char *)AUX_MU_IO_REG = buffer[i];
    }
}

void writeline_uart(const char *buffer, int count) {
    write_uart(buffer, count);
    write_uart("\n\r", 2);
}

int interact_readline_uart(char *buffer) {
    char c = '\0';
    int count = 0;
    while (c != NEWLINE) {
        while (!(*AUX_MU_LSR_REG & DATA_READY_BIT)) __wfe();
        c = *AUX_MU_IO_REG & 0xff;
        buffer[count++] = c;
        if (c != NEWLINE) {
            write_uart(&c, 1);
        } else {
            write_uart("\n\r", 2);
        }
    }

    buffer[count-1] = '\0';
    return count;
}

void puts_uart(const char *buffer) {
    int i = 0;
    while (buffer[i] != '\0') {
        while (!(*AUX_MU_LSR_REG & TRANSMITTER_EMPTY_BIT)) __wfe();
        *(char *)AUX_MU_IO_REG = buffer[i];
        i++;
    }
    write_uart("\n\r", 2);
}

void write_num_uart(unsigned long num) {
  int n = 0;
  char buf[20];
  while (num) {
    buf[n++] = (num % 10) + '0';
    num /= 10;
  }
  for (int i = n-1; i >= 0; --i) {
    write_uart(&buf[i], 1);
  }
}
