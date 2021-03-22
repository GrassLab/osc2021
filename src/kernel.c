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
  puts("Lab 3:");

  reserve_mem((void *)0x0, 0x1000);                         // spin table
  reserve_mem((void *)0x60000, 0x20000);                    // stack
  reserve_mem((void *)(&kn_start), (&kn_end - &kn_start));  // kernel
  reserve_mem((void *)(&kn_end), mem_size / PAGE_SIZE);     // buddy system
  reserve_mem((void *)0x3f000000, 0x1000000);               // MMIO

  // reserve_mem((void *)INIT_HEAP_START, INIT_HEAP_SIZE);     // 64 page heap

  // init_heap((void *)INIT_HEAP_START, INIT_HEAP_SIZE);
  init_buddy((char *)(&kn_end));

  kfree((void *)0x3f000000);
  kfree((void *)0x0);
  kfree((void *)0x60000);
  kfree((void *)(&kn_start));
  kfree((void *)(&kn_end));
  log_buddy();

  // init_rootfs(new_ramfs());

  // dentry root;
  // init_dentry(&root);

  // opendir("/", &root);
  // parse_initramfs(&root);
  // closedir(&root);

  shell();
}