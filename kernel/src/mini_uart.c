#include "mini_uart.h"

#include "gpio.h"

void uart_init() {
  register unsigned int r;
  r = *GPFSEL1;
  r &= ~((7 << 12) | (7 << 15));  // clean GPIO 14, GPIO 15
  r |= (2 << 12) | (2 << 15);     // set ALT5 for GPIO 14, GPIO 15
  *GPFSEL1 = r;

  *GPPUD = 0;  // disable GPIO pull up/down
  r = 150;
  while (r--) asm volatile("nop");     // delay 150 cycles
  *GPPUDCLK0 = (1 << 14) | (1 << 15);  // clock the signal
  r = 150;
  while (r--) asm volatile("nop");  // delay 150 cycles
  *GPPUDCLK0 = 0;                   // remove the clock

  /* initialize Mini UART */
  *AUXENB |= 1;          // enable mini UART
  *AUX_MU_CNTL_REG = 0;  // disable transmitter, receiver during configuration
  *AUX_MU_IER_REG = 1;   // enable receive interrupts
  *AUX_MU_LCR_REG = 3;   // enable 8 bit mode
  *AUX_MU_MCR_REG = 0;   // set RTS line to be always high
  *AUX_MU_BAUD = 270;    // set baud rate to 115200
  // comment this line to avoid weird character
  // *AUX_MU_IIR_REG = 0xc6;  // no FIFO
  *AUX_MU_CNTL_REG = 3;  // enable transmitter and receiver back

  read_buf_start = read_buf_end = 0;
  write_buf_start = write_buf_end = 0;
  enable_uart_interrupt();
}

void uart_send(unsigned int c) {
  // wait until we can send
  while (!(*AUX_MU_LSR_REG & 0x20)) {
    asm volatile("nop");
  }
  // write the character to the buffer
  *AUX_MU_IO_REG = c;
}

char uart_getb() {
  // wait until something is in the buffer
  while (!(*AUX_MU_LSR_REG & 0x01)) {
    asm volatile("nop");
  }
  // read character
  char r = (char)(*AUX_MU_IO_REG);
  // '\r' => '\n'
  return r;
}

char uart_getc() {
  // wait until something is in the buffer
  while (!(*AUX_MU_LSR_REG & 0x01)) {
    asm volatile("nop");
  }
  // read character
  char r = (char)(*AUX_MU_IO_REG);
  // '\r' => '\n'
  return r == '\r' ? '\n' : r;
}

void uart_puts(char *s) {
  while (*s) {
    // convert '\n' to carrige '\r' and '\n'
    if (*s == '\n') uart_send('\r');
    uart_send(*s++);
  }
}

void enable_uart_interrupt() { *ENABLE_IRQS_1 = AUX_IRQ; }

void disable_uart_interrupt() { *DISABLE_IRQS_1 = AUX_IRQ; }

void enable_transmit_interrupt() { *AUX_MU_IER_REG |= 0x2; }

void disable_transmit_interrupt() { *AUX_MU_IER_REG &= ~(0x2); }

void uart_handler() {
  disable_uart_interrupt();
  int is_read = (*AUX_MU_IIR_REG & 0x4);
  int is_write = (*AUX_MU_IIR_REG & 0x2);

  if (is_read) {
    char c = (char)(*AUX_MU_IO_REG);
    read_buf[read_buf_end++] = c;
    if (read_buf_end == UART_BUFFER_SIZE) read_buf_end = 0;
  } else if (is_write) {
    while (*AUX_MU_LSR_REG & 0x20) {
      if (write_buf_start == write_buf_end) {
        disable_transmit_interrupt();
        break;
      }
      char c = write_buf[write_buf_start++];
      *AUX_MU_IO_REG = c;
      if (write_buf_start == UART_BUFFER_SIZE) write_buf_start = 0;
    }
  }
  enable_uart_interrupt();
}

char uart_async_getc() {
  // wait until there are new data
  while (read_buf_start == read_buf_end) {
    asm volatile("nop");
  }
  char c = read_buf[read_buf_start++];
  if (read_buf_start == UART_BUFFER_SIZE) read_buf_start = 0;
  // '\r' => '\n'
  return c == '\r' ? '\n' : c;
}

void uart_async_puts(char *str) {
  for (int i = 0; str[i]; i++) {
    if (str[i] == '\n') write_buf[write_buf_end++] = '\r';
    write_buf[write_buf_end++] = str[i];
    if (write_buf_end == UART_BUFFER_SIZE) write_buf_end = 0;
  }
  enable_transmit_interrupt();
}
