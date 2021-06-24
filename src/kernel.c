#include "cpio.h"
#include "exc.h"
#include "gpio.h"
#include "io.h"
#include "mem.h"
#include "reset.h"
#include "sched.h"
#include "timer.h"
#include "tmpfs.h"
#include "util.h"
#include "vfs.h"

extern unsigned char kn_start, kn_end;

void shell() {
  char cmd_buf[1024];
  while (1) {
    schedule();
    gets_n(cmd_buf, 1023);
    if (!strcmp(cmd_buf, "reboot")) {
      reset(5);
    } else if (!strcmp_n(cmd_buf, "exec ", 5)) {
      if (clone() == 0) {
        exec(cmd_buf + 5);
      } else {
        wait();
      }
    } else if (strcmp(cmd_buf, "")) {
      print("command not found: ");
      puts(cmd_buf);
    }
  }
}

void kernel() {
  // void *dtb_addr = *(void **)(0x20000);

  el2_to_el1_preserve_sp();
  set_el1_evt();

  puts("Lab 6:");

  reserve_mem((void *)0x0, 0x1000);  // spin table
  reserve_mem((void *)0x80000 - KERN_STACK_SIZE, KERN_STACK_SIZE);  // stack
  reserve_mem((void *)(&kn_start), (&kn_end - &kn_start));          // kernel
  reserve_mem((void *)0x3f000000, 0x1000000);                       // MMIO
  reserve_cpio();

  init_kmalloc();

  enable_interrupt();
  core_timer_enable();
  init_nonblock_io();

  init_vfs();
  init_sched();

  tick_rate = 0.001;
  tick();

  setup_tmpfs();
  vfs_mount("tmpfs", "/", "tmpfs");

  thread_create(&pop_cpio);
  thread_create(&shell);
  idle();
}