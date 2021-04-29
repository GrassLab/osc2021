#include "cpio.h"

#include <stddef.h>

#include "io.h"
#include "mem.h"
#include "util.h"

void *initramfs = (void *)0x8000000;

typedef struct cpio_newc_header {
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

void *get_cpio_file(const char *name) {
  cpio_newc_header *cpio_itr = (cpio_newc_header *)initramfs;
  while (strcmp_n(cpio_itr->c_magic, "070701", 6) == 0) {
    unsigned long mode = atol_n(cpio_itr->c_mode, 8, 16);
    if (mode == 0 && strcmp(cpio_itr->data, "TRAILER!!!") == 0) {
      return NULL;
    }
    if (strcmp(name, cpio_itr->data) == 0) {
      return (void *)cpio_itr;
    }
    unsigned long filesize = atol_n(cpio_itr->c_filesize, 8, 16);
    unsigned long namesize = atol_n(cpio_itr->c_namesize, 8, 16);
    cpio_itr = (cpio_newc_header *)&(
        cpio_itr->data[2 + pad(namesize - 2, 4) + pad(filesize, 4)]);
  }
  return NULL;
}

void reserve_cpio() {
  cpio_newc_header *cpio_itr = (cpio_newc_header *)initramfs;
  unsigned long cpio_end;
  while (strcmp_n(cpio_itr->c_magic, "070701", 6) == 0) {
    unsigned long filesize = atol_n(cpio_itr->c_filesize, 8, 16);
    unsigned long namesize = atol_n(cpio_itr->c_namesize, 8, 16);
    unsigned long mode = atol_n(cpio_itr->c_mode, 8, 16);
    if (mode == 0 && strcmp(cpio_itr->data, "TRAILER!!!") == 0) {
      cpio_end = (unsigned long)&(cpio_itr->data[10]);
      reserve_mem((void *)initramfs,
                  pad(cpio_end - (unsigned long)initramfs, 4096));
    }
    cpio_itr = (cpio_newc_header *)&(
        cpio_itr->data[2 + pad(namesize - 2, 4) + pad(filesize, 4)]);
  }
}

unsigned long get_file_size(const void *cpio_file) {
  return atol_n(((cpio_newc_header *)cpio_file)->c_filesize, 8, 16);
}

void *get_file_data(const void *cpio_file) {
  unsigned long namesize =
      atol_n(((cpio_newc_header *)cpio_file)->c_namesize, 8, 16);
  return &(((cpio_newc_header *)cpio_file)->data[2 + pad(namesize - 2, 4)]);
}