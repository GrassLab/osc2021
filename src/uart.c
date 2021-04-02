#include "uart.h"

#include "gpio.h"

#define AUX_ENABLE ((volatile unsigned int *)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO ((volatile unsigned int *)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER ((volatile unsigned int *)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR ((volatile unsigned int *)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR ((volatile unsigned int *)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR ((volatile unsigned int *)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR ((volatile unsigned int *)(MMIO_BASE + 0x00215054))
#define AUX_MU_MSR ((volatile unsigned int *)(MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH ((volatile unsigned int *)(MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL ((volatile unsigned int *)(MMIO_BASE + 0x00215060))
#define AUX_MU_STAT ((volatile unsigned int *)(MMIO_BASE + 0x00215064))
#define AUX_MU_BAUD ((volatile unsigned int *)(MMIO_BASE + 0x00215068))

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

unsigned char uart_recv_c() {
  while ((*AUX_MU_LSR & 0x1) == 0)
    ;
  return (unsigned char)(*AUX_MU_IO);
}

void uart_send_c(unsigned char data) {
  while ((*AUX_MU_LSR & 0x20) == 0)
    ;
  *AUX_MU_IO = (unsigned int)data;
}

void flush() {
  while ((*AUX_MU_STAT & (1 << 9)) == 0)
    ;
}

unsigned int uart_recv_i() {
  unsigned int data;
  unsigned char *itr = (unsigned char *)&data;
  for (int i = 0; i < 4; i++) {
    *itr++ = uart_recv_c();
  }
  return data;
}

// blocking uart recv unsigned long
unsigned long uart_recv_l() {
  unsigned long data;
  unsigned char *itr = (unsigned char *)&data;
  for (int i = 0; i < 8; i++) {
    *itr++ = uart_recv_c();
  }
  return data;
}

// blocking uart send unsigned int
void uart_send_i(unsigned int data) {
  unsigned char *itr = (unsigned char *)&data;
  for (int i = 0; i < 4; i++) {
    uart_send_c(*itr++);
  }
}

// blocking uart send unsigned long
void uart_send_l(unsigned long data) {
  unsigned char *itr = (unsigned char *)&data;
  for (int i = 0; i < 8; i++) {
    uart_send_c(*itr++);
  }
}
