#include <stdint.h>
#include <stddef.h>
#include "include/uart.h"
#include "include/string.h"
#include "include/cpio.h"

static unsigned int to_int(char c) {
  if (c >= '0' && c <= '9') return      c - '0';
  if (c >= 'A' && c <= 'F') return 10 + c - 'A';
  if (c >= 'a' && c <= 'f') return 10 + c - 'a';
  return -1;
}

static unsigned int hex_to_int(const char *buf, int len) {
  unsigned out = 0;
  for (int i = 0; i < len; ++i) {
    out <<= 4;
    out += to_int(buf[i]);
  }
  return out;
}

static size_t align_up(size_t size, int alignment) {
  return (size + alignment - 1) & -alignment;
}

void cpio_list_file(const char *cpio_buf) {
  uintptr_t ptr = (uintptr_t)cpio_buf;
  const struct cpio_newc_header *hdr;

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

const char *cpio_get_content(const char *path, const char *cpio_buf) {
  uintptr_t ptr = (uintptr_t)cpio_buf;
  const struct cpio_newc_header *hdr;

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
    unsigned mode     = hex_to_int(hdr->c_mode, 8);

    if (!strcmp(hdr->content_buf, path)) {
      if (S_ISDIR(mode))
        return "directory";

      if (filesize) {
        ptr += sizeof(struct cpio_newc_header);
        ptr = align_up(ptr + namesize, 4);
        return (char *)ptr;
      }
      return "";

    } else {
      ptr += sizeof(struct cpio_newc_header);
      ptr = align_up(ptr + namesize, 4);
      ptr = align_up(ptr + filesize, 4);
    }
  }
}