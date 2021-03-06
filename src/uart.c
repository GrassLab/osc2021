#include "uart.h"

#include "gpio.h"
#include "util.h"

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
  r &= ~((7 << 12) | (7 << 15));  // clear gpio14, gpio15 setting
  r |= (2 << 12) | (2 << 15);     // set alt5
  *GPFSEL1 = r;
  *GPPUD = 0;  // disable pull-up/down
  wait_clock(150);
  *GPPUDCLK0 = (1 << 14) | (1 << 15);  // set to gpio14, gpio15
  wait_clock(150);
  *GPPUDCLK0 = 0;    // flush GPIO setup
  *AUX_MU_CNTL = 3;  // enable Tx Rx
}

unsigned int uart_get_raw() {
  while ((*AUX_MU_LSR & 0x1) == 0)
    ;
  return (*AUX_MU_IO);
}

void uart_put_raw(unsigned int data) {
  while ((*AUX_MU_LSR & 0x20) == 0)
    ;
  *AUX_MU_IO = data;
}

void uart_flush_stdin() {
  unsigned int d;
  while ((*AUX_MU_LSR & 0x1) != 0) {
    d = (*AUX_MU_IO);
  }
}