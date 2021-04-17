#ifndef __CPIO_H_
#define __CPIO_H_

#ifdef _QEMU
#define INITRAMFS_BASE 0x8000000
#else
#define INITRAMFS_BASE 0x20000000
#endif // _QEMU

#define NEW_ASCII_FORMAT "070701"
#define FILE_TYPE_MASK 0170000
#define FILE_TYPE_SOCKET 0140000
#define FILE_TYPE_SYMBO 0120000
#define FILE_TYPE_FILE 0100000
#define FILE_TYPE_DEVICE 0060000
#define FILE_TYPE_DIR 0040000
#define FILE_TYPE_CHARDEV 0020000
#define FILE_TYPE_FIFO 0010000
#define FILE_TYPE_SUID 0004000
#define FILE_TYPE_SGID 0002000
#define FILE_TYPE_STICKY 0001000
#define FILE_TYPE_PERM 0000777
#define CPIO_NEWC_HDR_OFFSET 110

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
};

struct cpio_file {
  struct cpio_newc_header *hdr;
  char *pathname;
  char *content;
};

int cpio_parse(const char *, const int, const char *);
int initramfs_ls();
int initramfs_cat(const char *);

#endif // __CPIO_H_
