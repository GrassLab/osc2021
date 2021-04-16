#include <stdint.h>
#include <stddef.h>
#include "printf.h"
#include "string.h"
#include "cpio.h"
#include "mm.h"

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

  puts("Archived files:");
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
    puts((char *)ptr);
    ptr = align_up(ptr + namesize, 4);
    ptr = align_up(ptr + filesize, 4);
  }
}

/* TODO: for future implementation, it should be mounted first */
struct file *cpio_get_file(const char *path, const char *cpio_buf, const char *cpio_end) {
  uintptr_t ptr = (uintptr_t)cpio_buf;
  const struct cpio_newc_header *hdr;

  while (ptr < (uintptr_t)cpio_end) {
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
      struct file *f = kmalloc(sizeof(struct file));

      if (S_ISDIR(mode)) {
        f->data = NULL;
        f->f_mode = mode;
        f->size = 0;
      } else {
        ptr += sizeof(struct cpio_newc_header);
        ptr = align_up(ptr + namesize, 4);

        char *data = NULL;
        if (filesize) {
          data = kmalloc(filesize);
          memcpy(data, (char *)ptr, filesize);
        }

        f->data = data;
        f->f_mode = mode;
        f->size = filesize;
      }
      return f;

    } else {
      ptr += sizeof(struct cpio_newc_header);
      ptr = align_up(ptr + namesize, 4);
      ptr = align_up(ptr + filesize, 4);
    }
  }

  return NULL;
}