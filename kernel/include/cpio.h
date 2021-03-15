#pragma once

#define RAMFS_ADDR 0x8000000
#define CPIO_MAGIC "070701"
#define CPIO_END "TRAILER!!!"

typedef struct {
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
} cpio_newc_header;

unsigned long long align_up(unsigned long long addr,
                            unsigned long long alignment);
void cpio_ls();
void cpio_cat(char *filename);
