#include <stdint.h>
#include <stddef.h>
#include <printf.h>
#include <string.h>
#include <fs/cpio.h>
#include <mm.h>
#include <dtb.h>

static const char *cpio_buf, *cpio_end;

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

static uint32_t get_be_int(const void *ptr) {
    return __builtin_bswap32(*(uint32_t *)ptr);
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

/* TODO: build a tree structure */
void init_cpio_storage(dtb_node *dtb) {
  dtb_prop *prop = find_device_tree(dtb, "/chosen/linux,initrd-start");
  if (prop) {
    cpio_buf = (char *)(uintptr_t)get_be_int(prop->data);
    printf("[Kernel] initrd: 0x%p\n\r", cpio_buf);
  }
  prop = find_device_tree(dtb, "/chosen/linux,initrd-end");
  if (prop) {
    cpio_end = (char *)(uintptr_t)get_be_int(prop->data);
  }
}

/* TODO: for future implementation, it should be mounted first */
struct file *cpio_get_file(const char *path) {
  if (!cpio_buf || !cpio_end) {
    return NULL;
  }

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