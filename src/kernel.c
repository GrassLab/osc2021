#include "cpio.h"
#include "exc.h"
#include "gpio.h"
#include "io.h"
#include "mem.h"
#include "ramfs.h"
#include "reset.h"
#include "timer.h"
#include "util.h"
#include "vfs.h"

extern unsigned char kn_start, kn_end;

void print_proxy(char *s) {
  print(s);
  putc('\n');
  kfree(s);
}

void shell() {
  // dentry dir;
  // init_dentry(&dir);
  // opendir("/", &dir);
  char cmd_buf[1024];

  while (1) {
    gets_n(cmd_buf, 1023);
    if (!strcmp(cmd_buf, "help")) {
      print(
          "help   : help menu\n"
          "reboot : reboot system\n"
          "cat    : print content\n"
          "ls     : listt directory\n"
          "cd     : change directory\n");
    } else if (!strcmp(cmd_buf, "reboot")) {
      reset(5);
    } else if (!strcmp_n(cmd_buf, "cat ", 4)) {
      // struct file f;
      // char read_buf[4096];
      // open(cmd_buf + 4, &dir, &f, 0);
      // int read_len;
      // while ((read_len = read(&f, read_buf, 4096) > 0)) {
      //   print_n(read_buf, read_len);
      // }
      // close(&f);
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
        unsigned long file_size = get_file_size(cpio_file);
        void *file_data = get_file_data(cpio_file);
        void *usr_prog = kmalloc(pad(file_size, 4096));
        void *usr_stack = kmalloc(PAGE_SIZE * 2);
        memcpy(usr_prog, file_data, file_size);
        exec_usr(usr_prog, usr_stack, 0);
        kfree(usr_stack);
        kfree(usr_prog);
      } else {
        print("program not found\n");
      }
    } else if (!strcmp_n(cmd_buf, "setTimeout ", 11)) {
      char *timeout = split_str(cmd_buf + 11);
      add_timer((double)(atol(timeout)), &print_proxy, new_str(cmd_buf + 11));
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

  puts("Lab 4:");

  reserve_mem((void *)0x0, 0x1000);                         // spin table
  reserve_mem((void *)0x60000, 0x20000);                    // stack
  reserve_mem((void *)(&kn_start), (&kn_end - &kn_start));  // kernel
  reserve_mem((void *)0x3f000000, 0x1000000);               // MMIO
  reserve_cpio();

  init_kmalloc();

  enable_interrupt();

  core_timer_enable();

  unsigned long elapse = get_timer_cnt();
  print("abcdefghijklmnop\n");
  elapse = get_timer_cnt() - elapse;
  log_hex("sync io elapse", elapse, LOG_PRINT);

  init_nonblock_io();

  elapse = get_timer_cnt();
  print("abcdefghijklmnop\n");
  elapse = get_timer_cnt() - elapse;
  log_hex("async io elapse", elapse, LOG_PRINT);

  unsigned long tc = get_timer_cnt();
  tc = tc + timer_frq * 2;
  _add_timer(tc, &print_time, (void *)tc);

  disable_interrupt();
  add_task(&wait_clock, (void *)100000000, 2);
  enable_interrupt();
  log("fin stall\n", LOG_PRINT);

  add_timer(3, &print, (void *)"3\n");
  add_timer(2, &print, (void *)"2\n");
  add_timer(1, &print, (void *)"1\n");
  add_timer(5, &print, (void *)"5\n");

  // // init_rootfs(new_ramfs());
  // // dentry root;
  // // init_dentry(&root);

  // // opendir("/", &root);
  // // parse_initramfs(&root);
  // // closedir(&root);

  shell();
}