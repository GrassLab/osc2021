
#ifndef __CPIO_H_
#define __CPIO_H_

#include "stat.h"

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
const char *cpio_get_content(const char *path, const char *cpio_buf);
void cpio_load_program(char *target, uint64_t target_addr, void *cpiobuf)
#endif
