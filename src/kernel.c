#include "cpio.h"
#include "exc.h"
#include "gpio.h"
#include "io.h"
#include "mem.h"
#include "ramfs.h"
#include "reset.h"
#include "sched.h"
#include "timer.h"
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
    } else if (!strcmp_n(cmd_buf, "cat ", 4)) {
      void *cpio_file = get_cpio_file(cmd_buf + 4);
      if (cpio_file != NULL) {
        unsigned long file_size = get_file_size(cpio_file);
        void *file_data = get_file_data(cpio_file);
        print_n((char *)file_data, file_size);
      } else {
        print("file not found\n");
      }
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

void test_vfs() {
  clone();
  asm volatile("":::"memory");
  // log_hex("ts", (unsigned long)(get_taskstruct()), LOG_PRINT);
  log_hex("wd", (unsigned long)(get_taskstruct()->pwd), LOG_PRINT);
  dentry *od;
  if(vfs_opendent(&od, "") < 0) {
    log("error vfs", LOG_ERROR);
  }
  dentry *gr = get_vfs_root();
  log_hex("od", (unsigned long)od, LOG_PRINT);
  log_hex("gr", (unsigned long)gr, LOG_PRINT);
  vfs_closedent(od);
  vfs_closedent(gr);

  if(vfs_opendent(&od, "") < 0) {
    log("error vfs2", LOG_ERROR);
  }
  log_hex("od2", (unsigned long)od, LOG_PRINT);
  vfs_closedent(od);
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

  thread_create(&test_vfs);
  thread_create(&test_vfs);
  thread_create(&shell);
  idle();
}