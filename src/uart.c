#include "uart.h"

#include "gpio.h"

void wait_clock(register unsigned int t) {
  t >>= 2;
  while (t--)
    ;
}

void uart_init() {
  register unsigned int r;

  /* initialize UART */
  *AUX_ENABLE |= 1;    // enable UART1, AUX mini uart
  *AUX_MU_CNTL = 0;    // close Tx Rx
  *AUX_MU_LCR = 3;     // data size 8 bits
  *AUX_MU_MCR = 0;     // no auto flow control
  *AUX_MU_IER = 0;     // disable interrupt - enable bit
  *AUX_MU_IIR = 0xc6;  // disable interrupts - clear FIFO
  *AUX_MU_BAUD = 270;  // 115200 baud

  /* map UART1 to GPIO pins */
  r = *GPFSEL1;
  r &= ~((7 << 12) | (7 << 15));  // clear gpio14, gpio15
  r |= (2 << 12) | (2 << 15);     // set alt5
  *GPFSEL1 = r;
  *GPPUD = 0;  // disable pull-up/down
  wait_clock(150);
  *GPPUDCLK0 = (1 << 14) | (1 << 15);  // set to gpio14, gpio15
  wait_clock(150);
  *GPPUDCLK0 = 0;    // flush GPIO setup
  *AUX_MU_CNTL = 3;  // enable Tx Rx
}

void uart_putc(unsigned int c) {
  if (c == '\n') {
    uart_putc('\r');
  }
  while ((*AUX_MU_LSR & 0x20) == 0)
    ;
  *AUX_MU_IO = c;
}

char uart_getc() {
  char c;
  while ((*AUX_MU_LSR & 0x1) == 0)
    ;
  c = (char)(*AUX_MU_IO);
  // write back
  if (c == 127) {
    c = '\b';
  } else if (c == '\r') {
    uart_putc('\n');
    c = '\n';
  } else {
    uart_putc(c);
  }
  return c;
}

void uart_puts(const char *s) {
  while (*s) {
    uart_putc(*s++);
  }
}

void uart_gets(char *s, unsigned int max_length) {
  unsigned int cursor = 0;
  char c;
  while ((c = uart_getc()) != '\n' && cursor < max_length) {
    if (c == '\b') {
      if (cursor > 0) {
        uart_puts("\b \b");
        cursor--;
      }
    } else {
      s[cursor] = c;
      cursor++;
    }
  }
  s[cursor] = 0;
}