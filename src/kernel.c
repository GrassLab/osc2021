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
      void *cpio_file = get_cpio_file(cmd_buf + 5);
      if (cpio_file != NULL) {
        if (clone()) {
          unsigned long file_size = get_file_size(cpio_file);
          void *file_data = get_file_data(cpio_file);
          void *usr_prog = kmalloc(pad(file_size, 4096));
          void *usr_stack = kmalloc(KERN_STACK_SIZE) + KERN_STACK_SIZE;
          memcpy(usr_prog, file_data, file_size);
          exec_usr(usr_prog, usr_stack, 0);
          kfree(usr_stack);
          kfree(usr_prog);
        }
      } else {
        print("program not found\n");
      }
    } else if (strcmp(cmd_buf, "")) {
      print("command not found: ");
      puts(cmd_buf);
    }
  }
}

void print_pid() {
  clone();
  unsigned long pid = get_pid();
  log_hex("st", pid, LOG_PRINT);
  // wait_clock(1000000);
  sleep(5);
  die();
}

void kernel() {
  // void *dtb_addr = *(void **)(0x20000);

  el2_to_el1_preserve_sp();
  set_el1_evt();

  puts("Lab 5:");

  reserve_mem((void *)0x0, 0x1000);  // spin table
  reserve_mem((void *)0x80000 - KERN_STACK_SIZE, KERN_STACK_SIZE);  // stack
  reserve_mem((void *)(&kn_start), (&kn_end - &kn_start));          // kernel
  reserve_mem((void *)0x3f000000, 0x1000000);                       // MMIO
  reserve_cpio();

  init_kmalloc();

  enable_interrupt();
  core_timer_enable();
  init_nonblock_io();

  init_sched();

  tick_rate = 0.001;
  tick();

  for (int i = 0; i < 5; i++) {
    thread_create(&print_pid);
  }
  thread_create(&shell);
  idle();
}