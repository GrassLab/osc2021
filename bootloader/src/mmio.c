#include "mmio.h"

void reset(int tick) { // reboot after watchdog timer expire
  *((unsigned int *)PM_RSTC) = PM_PASSWORD | 0x20; // full reset
  *((unsigned int *)PM_WDOG) = PM_PASSWORD | tick; // number of watchdog tick
}

// void cancel_reset() {
//    set(PM_RSTC, PM_PASSWORD | 0); // full reset
//    set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
//}

void uart_init() {
  int cycle = 0;
  // GPIO
  *GPFSEL1 &= ~(0b111 << 12); // clear GPIO pin 14 alnt function
  *GPFSEL1 |= (0b010 << 12);  // GPIO Pin 14 takes alternate function 5
  *GPFSEL1 &= ~(0b111 << 15); // clear GPIO pin 15 alt function
  *GPFSEL1 |= (0b010 << 15);  // GPIO Pin 15 takes alternate function 5
  *GPPUD = 0b00;              // Disable GPIO pull up/down.
  cycle = 150;
  while (cycle--) {
    asm volatile("nop");
  }                          // busy waiting for 150 cycle
  *GPPUDCLK0 = (0b11 << 14); // trigger ping 14, 15
  cycle = 150;
  while (cycle--) {
    asm volatile("nop");
  }
  *GPPUD = 0;
  *GPPUDCLK0 = 0;
  // UART
  *AUX_ENABLES |= 1; // enable mini UART.
  *AUX_MU_CNTL_REG =
      0; // Disable transmitter and receiver during configuration.
  *AUX_MU_IER_REG =
      0; // Disable interrupt because currently you don’t need interrupt.
  *AUX_MU_LCR_REG = 3;    // Set the data size to 8 bit.
  *AUX_MU_MCR_REG = 0;    // Don’t need auto flow control.
  *AUX_MU_BAUD_REG = 270; // Set baud rate to 115200
  *AUX_MU_IIR_REG = 6;    // No FIFO.
  *AUX_MU_CNTL_REG = 3;   // Enable the transmitter and receiver.
  return;
}

char uart_getc() {
  char c;
  while (!(*AUX_MU_LSR_REG & 0b1)) {
    asm volatile("nop");
  }
  c = (char)*AUX_MU_IO_REG;
  // asm volatile("nop");
  return c;
}

void uart_setc(char c) {
  while (!(*AUX_MU_LSR_REG & 0b1 << 5)) {
    asm volatile("nop");
  }
  *AUX_MU_IO_REG = c;
  return;
}
