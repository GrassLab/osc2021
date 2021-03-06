#include "io.h"
#include "uart.h"
#include "util.h"

void recv_section() {
  puts("ST");
  unsigned char *section_addr = (unsigned char *)recv_ll();
  unsigned long section_size = recv_l();
  unsigned char checksum = 0;
  unsigned char prev_checksum;
  unsigned char recv_status;
  unsigned long i = 0;
  // test mem
  for (i = 0; i < section_size; i++) {
    *section_addr = 0x66;
    if (*section_addr != 0x66) {
      system_error("mem err\n");
    }
  }

  if ((recv_status = uart_get_raw()) == 'A') {
    for (i = 0; i < section_size; i++) {
      *section_addr++ = 0;
    }
  } else if (recv_status == 'D') {
    for (i = 0; i < section_size; i++) {
      *section_addr = uart_get_raw();
      checksum = *section_addr++;
      if (1) {
        uart_flush_stdin();
        uart_put_raw(checksum);
        while (uart_get_raw() != 0x69)
          ;
        if (uart_get_raw()) {
          i -= 1;
          section_addr -= 1;
          checksum = prev_checksum;
          wait_clock(1000000);
        } else {
          prev_checksum = checksum;
        }
      }
    }
  } else {
    system_error("control flow failed\n");
  }
  uart_put_raw(checksum);
  send_l(i);
  puts("FN");
}

void bootloader() {
  uart_init();

  char recv_status = 0;
  // wait for start
  while ((recv_status = uart_get_raw()) != 's')
    ;

  // recv all section
  while ((recv_status = uart_get_raw()) == 'S') {
    recv_section();
  }

  // recv kernel address
  if (recv_status != 'K') {
    system_error("control flow failed\n");
  }

  void (*kernel)(void) = (void *)recv_ll();

  // end transmission
  if ((recv_status = uart_get_raw()) != 'E') {
    system_error("control flow failed\n");
  }

  kernel();
}