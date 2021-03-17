#include "io.h"
#include "mem.h"
#include "ramfs.h"
#include "reset.h"
#include "util.h"
#include "vfs.h"

void shell() {
  char cmd_buf[1024];
  // void *current_dir = get_root_dir();

  while (1) {
    gets_n(cmd_buf, 1023);
    if (!strcmp(cmd_buf, "help")) {
      puts(
          "help   : help menu\n"
          "reboot : reboot system\n");
    } else if (!strcmp(cmd_buf, "reboot")) {
      reset(5);
    } else if (strcmp(cmd_buf, "")) {
      puts("no such instruction \'");
      puts(cmd_buf);
      puts("\'\n");
    }
  }
}

void kernel() {
  puts("Lab 2:\n\n");
  init_rootfs(new_ramfs());
  struct dentry root;
  init_dentry(&root);
  vfs_get_dir("/", &root);
  parse_initramfs(&root);
  shell();
}