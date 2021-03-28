#include "include/dtb.h"
#include "include/string.h"

typedef struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
} fdt_header;

static uint32_t get_be_int(void *ptr) {
    unsigned char *bytes = ptr;
    uint32_t ret = bytes[3];
    ret |= bytes[2] << 8;
    ret |= bytes[1] << 16;
    ret |= bytes[0] << 24;

    return ret;
}

static uint32_t align_up(uint32_t size, int alignment) {
  return (size + alignment - 1) & -alignment;
}

static int parse_struct(uintptr_t ptr, uintptr_t strings, uint32_t totalsize, dtb_callback cb) {
  uintptr_t end = ptr + totalsize;

  while (ptr < end) {
    uint32_t token = get_be_int((char *)ptr);
    ptr += 4;

    switch (token) {
      case FDT_BEGIN_NODE:
        cb(FDT_BEGIN_NODE, (char *)ptr, NULL, 0);
        ptr += align_up(strlen((char *)ptr) + 1, 4);    
        break;

      case FDT_END_NODE:
        cb(FDT_END_NODE, NULL, NULL, 0);
        break;

      case FDT_NOP:
        cb(FDT_NOP, NULL, NULL, 0);
        break;

      case FDT_PROP: {
        uint32_t size = get_be_int((char *)ptr);
        ptr += 4;
        uint32_t nameoff = get_be_int((char *)ptr);
        ptr += 4;
        cb(FDT_PROP, (char *)(strings + nameoff), (void *)ptr, size);
        ptr += align_up(size, 4);
        break;
      }

      case FDT_END:
        cb(FDT_END, NULL, NULL, 0);
        return 0;

      default:
        return -1;
    }
  }

  return -1;
}

int traverse_device_tree(void *_dtb, dtb_callback cb) {
  uintptr_t dtb = (uintptr_t)_dtb;
  fdt_header *header = (fdt_header *)dtb;
  if (get_be_int(&header->magic) != 0xd00dfeed) {
    return -1;
  }
  uint32_t totalsize = get_be_int(&header->totalsize);
  uintptr_t dt_struct_ptr = dtb + get_be_int(&header->off_dt_struct);
  uintptr_t dt_strings_ptr = dtb + get_be_int(&header->off_dt_strings);
  /* TODO: also parse mem reservaion blocks */
  /* uintptr_t mem_rsvmap_ptr = dtb + get_be_int(&header->off_mem_rsvmap); */

  return parse_struct(dt_struct_ptr, dt_strings_ptr, totalsize, cb);
}