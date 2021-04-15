#include "cpio.h"
#include "util.h"

// input pathsize and output align length
size_t _cpio_align(size_t pathsize) {
  return (pathsize & 0xfffffffc) + !(!(pathsize & 3)) * 4;
  // return ((pathsize+2)/4 - !((pathsize+2)%4)) * 4 + 2;
}

unsigned int _byte2int(char arr[8]) {
  unsigned int ret = 0, cnt;
  for (cnt = 0; cnt < 8; ++cnt) {
    ret <<= 4;
    if (arr[cnt] <= '9' && arr[cnt] >= '0') {
      ret += arr[cnt] - '0';
    } else if (arr[cnt] <= 'F' && arr[cnt] >= 'A') {
      ret += arr[cnt] - 'A' + 10;
    } else {
      // ERROR, not 0-F
      return 0;
    }
  }
  return ret;
}

int cpio_parse(const char *base, const int cmd /* 1: list, 2: cat */,
               const char *cat_pathname) {
  // Check header before puting into structure
  if (strncmp(base, NEW_ASCII_FORMAT, 6)) {
    // return error
    // Not cpio new ascii format
    putln("File type error!");
    return 1;
  }
  struct cpio_file file;
  file.hdr = (struct cpio_newc_header *)base;
  // Check header magic number
  if (strncmp(file.hdr->c_magic, NEW_ASCII_FORMAT, 6)) {
    // not cpio new ascii format
    putln("File type error!");
    return 1;
  }

  // parse pathname
  file.pathname = (char *)(base + CPIO_NEWC_HDR_OFFSET);
  // parse content
  // if (_byte2int(file.hdr->c_filesize)) {
  file.content =
      (char *)((char *)base + CPIO_NEWC_HDR_OFFSET +
               _cpio_align(((size_t)(_byte2int(file.hdr->c_namesize))) + 2) -
               2);
  //} else {
  // file.content = NULL;
  //}

  // cmd
  if (cmd == 1 && strncmp(file.pathname, "TRAILER!!!",
                          (size_t)_byte2int(file.hdr->c_namesize))) {
    // ls
    putln(file.pathname);
  } else if (cmd == 2 && !strcmp(file.pathname, cat_pathname)) {
    // cat
    if (_byte2int(file.hdr->c_mode) & FILE_TYPE_FILE) {
      putln(file.content);
      return 0;
    } else {
      putln("This is not a regular file!");
      return 0;
    }
  }

  // Check if END of cpio fs
  if (strncmp(file.pathname, "TRAILER!!!",
              (size_t)_byte2int(file.hdr->c_namesize))) {
    // return 1, not end
    return cpio_parse(
        file.content + _cpio_align((size_t)(_byte2int(file.hdr->c_filesize))),
        cmd, cat_pathname);
  }

  return 0;
}

int initramfs_ls() {
  // read from INIT_RAMFS_BASE
  void *initramfs_base = (void *)INITRAMFS_BASE;
  return cpio_parse(initramfs_base, 1, NULL);
}

int initramfs_cat(const char *pathname) {
  void *initramfs_base = (void *)INITRAMFS_BASE;
  return cpio_parse(initramfs_base, 2, pathname);
}
