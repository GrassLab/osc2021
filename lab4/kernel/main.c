#include <uart.h>
#include <shell.h>
#include <string.h>
#include <mbox.h>
#include <types.h>
#include <cpio.h>
#include <devicetree.h>
#include <buddy.h>
#include <dynamic.h>
#include <printf.h>
void main(size_t dtb_address) {
  uart_init();
  init_printf(null, putc);
  devicetree_parse(dtb_address, DISPLAY_DEVICE_NAME, null);
  cpio_parse_newc_header(CPIO_ADDRESS);
  buddy_init();
  dynamic_init();
  shell();
}

