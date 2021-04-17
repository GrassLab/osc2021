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
#include <sched.h>

void main(void* dtb_address) {
  uart_init();
  buddy_init();
  dynamic_init();
  task_init();
  devicetree_parse(dtb_address, DISPLAY_DEVICE_NAME, null);
  cpio_parse_newc_header((void *)CPIO_ADDRESS);
  shell();
}

