#ifndef RAMFS_H
#define RAMFS_H

#include "vfs.h"

#define INITRAMFS 0x8000000

struct super_block *new_ramfs();

void parse_initramfs(struct dentry* root);

struct cpio_newc_header {
  char c_magic[6];
  char c_ino[8];
  char c_mode[8];
  char c_uid[8];
  char c_gid[8];
  char c_nlink[8];
  char c_mtime[8];
  char c_filesize[8];
  char c_devmajor[8];
  char c_devminor[8];
  char c_rdevmajor[8];
  char c_rdevminor[8];
  char c_namesize[8];
  char c_check[8];
  char data[2];
} cpio_newc_header;

#endif