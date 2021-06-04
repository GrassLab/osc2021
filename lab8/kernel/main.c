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
#include <vfs.h>
#include <fat32.h>
#include <sdhost.h>

extern void core_timer_enable();
extern void mmu_enable();

void main(void* dtb_address) {
  uart_init();
  //devicetree_parse(dtb_address, DISPLAY_DEVICE_NAME, null);
  cpio_parse_newc_header((void *)CPIO_ADDRESS);
  buddy_init();
  dynamic_init();
  //task_init();
  sd_init();
  root_fs_init();
  shell();
}

