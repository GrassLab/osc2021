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
    } else if (strcmp(cmd_buf, "")) {
      print("command not found: ");
      puts(cmd_buf);
    }
  }
}

void svc_test() {
  for (int i = 0; i < 5; i++) {
    asm volatile("svc 0;");
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

  init_kmalloc();

  enable_interrupt();

  core_timer_enable();
  add_timer(2, &print_time, NULL);

  add_timer(3, &print, (void *)"3\n");
  add_timer(2, &print, (void *)"2\n");
  add_timer(1, &print, (void *)"1\n");
  add_timer(5, &print, (void *)"5\n");

  void *usr_sp = kmalloc(PAGE_SIZE);

  exec_usr(&svc_test, usr_sp, 0x0);

  // // init_rootfs(new_ramfs());
  // // dentry root;
  // // init_dentry(&root);

  // // opendir("/", &root);
  // // parse_initramfs(&root);
  // // closedir(&root);

  shell();
}