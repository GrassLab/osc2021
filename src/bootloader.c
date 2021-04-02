#include "uart.h"

void recv_section() {
  uart_send_c('S');

  unsigned char *section_addr = (unsigned char *)uart_recv_l();
  unsigned long section_size = uart_recv_l();
  unsigned char section_type = uart_recv_c();
  unsigned char checksum = 0;

  if (section_type == 'A') {
    for (unsigned long i = 0; i < section_size; i += 8) {
      *(unsigned long *)section_addr = 0;
      section_addr += 8;
    }
  } else if (section_type == 'D') {
    for (unsigned long i = 0; i < section_size; i++) {
      *section_addr = uart_recv_c();
      checksum ^= *section_addr++;
    }
  }

  uart_send_c(checksum);
}

void *recv_kernel() {
  void *kernel;

  // wait for start
  while (uart_recv_c() != 's')
    ;

  // recv section
  while (uart_recv_c() == 'S') {
    recv_section();
  }

  // kernel entry
  kernel = (void *)uart_recv_l();

  return kernel;
}

void bootloader() {
  uart_init();
  void *kernel = recv_kernel();

  // branch to kernel
  asm volatile(
      "add x29, x29, #8;"
      "str %0, [x29];" ::"r"(kernel));
}