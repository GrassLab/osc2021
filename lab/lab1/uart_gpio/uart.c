
#include "uart.h"

#include "gpio.h"

void uart_init() {
  /* initialize UART */
  *AUX_ENABLE |= 1;  // enable UART1, AUX mini uart
  *AUX_MU_CNTL = 0;  // Disable enable RX & TX operation
  *AUX_MU_LCR = 3;   // used to set data bit length (8 bit = 3)
  *AUX_MU_MCR = 0;   // disable auto flow control
  *AUX_MU_IER = 0;   // disable interrupts
  // *AUX_MU_IIR = 0xc6; // identify TX or RX interrup
  *AUX_MU_BAUD = 270;  // 115200 baud

  /* map UART1 to GPIO pins */
  register unsigned int r = *GPFSEL1;
  r &= ~((7 << 12) | (7 << 15));  // gpio14, gpio15
  r |= (2 << 12) | (2 << 15);     // alt5
  *GPFSEL1 = r;
  *GPPUD = 0;  // enable pins 14 and 15
  r = 150;
  while (r--) asm volatile("nop");
  *GPPUDCLK0 = (1 << 14) | (1 << 15);
  r = 150;
  while (r--) asm volatile("nop");
  *GPPUDCLK0 = 0;    // flush GPIO setup
  *AUX_MU_CNTL = 3;  // enable Tx, Rx
}

/* Send a character */
void uart_send(unsigned int c) {
  /* wait until we can send */
  do {
    asm volatile("nop");
  } while (!(*AUX_MU_LSR & 0x20));

  *AUX_MU_IO = c;  // write the character to the buffer
}

/* Receive a character */
char uart_getc() {
  /* wait until something is in the buffer */
  do {
    asm volatile("nop");
  } while (!(*AUX_MU_LSR & 0x01));

  return (char)(*AUX_MU_IO);  // read it and return
}

/* Display a string */
void uart_puts(char *s) {
  while (*s) uart_send(*s++);  // convert newline to carrige return + newline
}