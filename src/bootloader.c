#include "bl_util.h"
#include "io.h"
#include "uart.h"

void recv_section() {
  uart_put_raw('S');

  unsigned char *section_addr = (unsigned char *)recv_ll();
  unsigned long section_size = recv_l();
  unsigned char section_type = uart_get_raw();
  unsigned char checksum = 0;

  if (section_type == 'A') {
    for (unsigned long i = 0; i < section_size; i++) {
      *section_addr++ = 0;
    }
  } else if (section_type == 'D') {
    for (unsigned long i = 0; i < section_size; i++) {
      *section_addr = uart_get_raw();
      checksum ^= *section_addr++;
    }
  } else {
    sys_error("recv control failed");
  }

  uart_put_raw(checksum);
  uart_put_raw('F');
}

void *recv_kernel() {
  void *kernel;
  char recv_status = 0;

  // wait for start
  while ((recv_status = uart_get_raw()) != 's')
    ;

  // recv all section
  while ((recv_status = uart_get_raw()) == 'S') {
    recv_section();
  }

  // recv kernel entry point
  if (recv_status != 'K') {
    sys_error("recv control failed");
  }

  kernel = (void *)recv_ll();

  // end transmission
  if (uart_get_raw() != 'E') {
    sys_error("recv control failed");
  }

  return kernel;
}

void bootloader() {
  uart_init();

  void *kernel = recv_kernel();

  // branch to kernel
  asm volatile(
      "mov x30, %0;"
      "mov x0, x10;"
      "mov x1, x11;"
      "mov x2, x12;"
      "mov x3, x13;"
      "ret" ::"r"(kernel));
}