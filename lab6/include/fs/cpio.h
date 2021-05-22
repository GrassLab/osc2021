#ifndef __FS_CPIO_H_
#define __FS_CPIO_H_

#include <stat.h>
#include <dtb.h>

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
  char content_buf[0];
} __attribute__((packed));

void cpio_list_file(const char *cpio_buf);
struct file *cpio_get_file(const char *path);
void init_cpio_storage(dtb_node *dtb);
void cpio_create_root();
void populate_rootfs();

#endif
