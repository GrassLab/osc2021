#include "io.h"
#include "mem.h"
#include "ramfs.h"
#include "reset.h"
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

void kernel() {
  void *dtb_addr = *(void **)(0x20000);

  log_hex("dtb address", (unsigned long)dtb_addr, LOG_DEBUG);
  puts("Lab 3:");

  reserve_mem((void *)0x0, 0x1000);                         // spin table
  reserve_mem((void *)0x60000, 0x20000);                    // stack
  reserve_mem((void *)(&kn_start), (&kn_end - &kn_start));  // kernel
  reserve_mem((void *)0x3f000000, 0x1000000);               // MMIO

  init_kmalloc();

  void *addr[6];
  for (int i = 0; i < 6; i++) {
    addr[i] = kmalloc(PAGE_SIZE / 2 - 11);
    log_hex("kmalloc addr", (unsigned long)addr[i], LOG_DEBUG);
  }
  for (int i = 0; i < 6; i++) {
    kfree(addr[i]);
  }
  for (int i = 0; i < 6; i++) {
    addr[i] = kmalloc(PAGE_SIZE / 2 - 11);
    log_hex("kmalloc addr", (unsigned long)addr[i], LOG_DEBUG);
  }
  for (int i = 0; i < 6; i++) {
    addr[i] = kmalloc(PAGE_SIZE / 2 - 11);
    log_hex("kmalloc addr", (unsigned long)addr[i], LOG_DEBUG);
  }

  // // init_rootfs(new_ramfs());
  // // dentry root;
  // // init_dentry(&root);

  // // opendir("/", &root);
  // // parse_initramfs(&root);
  // // closedir(&root);

  // log("float\n");
  // double a = 3.0;
  // double b = 1.3333333333333333;

  // double c = a * b;
  // log_hex((unsigned long long)c);

  // double d[50];
  // for (int i = 0; i < 50; i++) {
  //   d[i] = i;
  // }
  // for(int i = 0; i < 50; i++) {
  //   d[i] *= d[i];
  // }

  // for(int i = 0; i < 50; i++) {
  //   log_hex((unsigned long long)d[i]);
  // }

  shell();
}