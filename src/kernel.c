#include "io.h"
#include "mem.h"
#include "ramfs.h"
#include "reset.h"
#include "util.h"
#include "vfs.h"
#include "ramfs.h"

void shell(struct dentry *dir) {
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
    } else if(!strcmp_n(cmd_buf, "cat ", 4)) {
      struct file f;
      vfs_open(cmd_buf + 4, dir, &f);
      char read_buf[4096];
      int rl = vfs_read(&f, read_buf, 4096);
      read_buf[rl] = 0;
      puts(read_buf);
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
  shell(&root);
}