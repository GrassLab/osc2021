#include <stdint.h>
#include <stddef.h>
#include "include/uart.h"
#include "include/string.h"

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
};

unsigned int to_int(char c) {
  if (c >= '0' && c <= '9') return      c - '0';
  if (c >= 'A' && c <= 'F') return 10 + c - 'A';
  if (c >= 'a' && c <= 'f') return 10 + c - 'a';
  return -1;
}

unsigned int hex_to_int(char *buf, int len) {
  unsigned out = 0;
  for (int i = 0; i < len; ++i) {
    out <<= 2;
    out += to_int(buf[i]);
  }
  return out;
}

size_t align_up(size_t size, int alignment) {
  return (size + alignment - 1) & -alignment;
}

void cpio_list_file(void *cpio_buf) {
  uintptr_t ptr = (uintptr_t)cpio_buf;
  struct cpio_newc_header *hdr;

  puts_uart("Archived files:");
  while (1) {
    hdr = (struct cpio_newc_header *)ptr;
    if (strncmp(hdr->c_magic, "070701", 6)) {
      return;
    }

    unsigned namesize = hex_to_int(hdr->c_namesize, 8);
    unsigned filesize = hex_to_int(hdr->c_filesize, 8);
    if (!strcmp(hdr->content_buf, "TRAILER!!!")) {
      break;
    }

    ptr += sizeof(struct cpio_newc_header);
    puts_uart((char *)ptr);
    ptr = align_up(ptr + namesize, 4);
    ptr = align_up(ptr + filesize, 4);
  }
}

char *cpio_get_content(char *path, char *cpio_buf) {
  uintptr_t ptr = (uintptr_t)cpio_buf;
  struct cpio_newc_header *hdr;

  while (1) {
    hdr = (struct cpio_newc_header *)ptr;
    if (strncmp(hdr->c_magic, "070701", 6)) {
      return NULL;
    }
    if (!strcmp(hdr->content_buf, "TRAILER!!!")) {
      return NULL;
    }

    unsigned namesize = hex_to_int(hdr->c_namesize, 8);
    unsigned filesize = hex_to_int(hdr->c_filesize, 8);

    if (!strcmp(hdr->content_buf, path)) {
      ptr += sizeof(struct cpio_newc_header);
      ptr = align_up(ptr + namesize, 4);
      return (char *)ptr;

    } else {
      ptr += sizeof(struct cpio_newc_header);
      ptr = align_up(ptr + namesize, 4);
      ptr = align_up(ptr + filesize, 4);
    }
  }
}