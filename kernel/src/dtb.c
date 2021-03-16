#include "dtb.h"

#include "io.h"
#include "string.h"
#include "utils.h"

void dtb_parse() {
  uint64_t dtb_addr = *((uint64_t *)0x9000000);
  fdt_header *header = (fdt_header *)dtb_addr;
  print_h(dtb_addr);
  // print_s("\n");
  // print_h((header->magic));
  // print_s("\n");
  // print_h(be2le(header->magic));
  // print_s("\n");
  // print_h((header->totalsize));
  // print_s("\n");
  // print_h(be2le(header->totalsize));
  // print_s("\n");
  // print_h((header->off_dt_struct));
  // print_s("\n");
  // print_h(be2le(header->off_dt_struct));
  // print_s("\n");
  // print_h((header->off_dt_strings));
  // print_s("\n");
  // print_h(be2le(header->off_dt_strings));
  // print_s("\n");
  // print_h((header->size_dt_struct));
  // print_s("\n");
  // print_h(be2le(header->size_dt_struct));
  print_s("\n\n\n");

  uint64_t struct_addr = dtb_addr + be2le(header->off_dt_struct);
  uint64_t strings_addr = dtb_addr + be2le(header->off_dt_strings);
  dtb_parse_node(struct_addr, strings_addr);
}

void dtb_parse_node(uint64_t struct_addr, uint64_t strings_addr) {
  int depth = 0;
  while (1) {
    uint32_t token = be2le(dtb_read_int(&struct_addr));

    if (token == FDT_BEGIN_NODE) {
      for (int i = 0; i < depth; i++) print_s("    ");
      depth++;
      char *name = dtb_read_string(&struct_addr);
      print_s("node: ");
      print_s(name);
      print_s("\n");
      struct_addr = align_up((uint64_t)struct_addr, 4);
    } else if (token == FDT_END_NODE) {
      depth--;
    } else if (token == FDT_PROP) {
      struct_addr = print_property(struct_addr, strings_addr, depth);
    } else if (token == FDT_NOP) {
      continue;
    } else if (token == FDT_END) {
      break;
    }
  }
}

uint64_t print_property(uint64_t struct_addr, uint64_t strings_addr,
                        int depth) {
  uint32_t len = be2le(dtb_read_int(&struct_addr));
  uint32_t nameoff = be2le(dtb_read_int(&struct_addr));
  char *property = (char *)(strings_addr + nameoff);

  int value_type = 0;
  char *is_u32[] = {"phandle", "#address-cells", "#size-cells", "virtual-reg",
                    "interrupt-parent"};
  char *is_string[] = {"compatible", "model", "status", "name", "device_type"};

  for (int i = 0; i < sizeof(is_u32) / sizeof(is_u32[0]); i++) {
    if (strcmp(property, is_u32[i]) == 0) {
      value_type = 1;
      break;
    }
  }
  for (int i = 0; i < sizeof(is_string) / sizeof(is_string[0]); i++) {
    if (strcmp(property, is_string[i]) == 0) {
      value_type = 2;
      break;
    }
  }

  for (int i = 0; i < depth; i++) print_s("    ");
  print_s(property);
  print_s(" = ");

  if (value_type == 0) {  // hex, e.g. <0x4600 0x100>
    int count = 0;
    print_s("<");
    for (uint32_t i = 0; i < len; i += 4) {
      if (count > 0) print_s(" ");
      count++;
      uint32_t num = be2le(dtb_read_int(&struct_addr));
      print_h(num);
    }
    print_s(">");

    if (len > 0) {
      // struct_addr += len;
      // struct_addr = align_up((uint64_t)struct_addr, 4);
    }
  }
  if (value_type == 1) {  // int, e.g. <1>
    print_s("<");
    uint32_t num = be2le(dtb_read_int(&struct_addr));
    print_i(num);
    print_s(">");
  }
  if (value_type == 2) {  // string, e.g. "fsl,MPC8349EMITX"
    int count = 0;
    print_s("\"");
    for (uint32_t i = 0; i < len;) {
      if (count > 0) print_s(",");
      count++;
      char *string = (char *)(struct_addr + i);
      print_s(string);
      i += strlen(string) + 1;
    }
    print_s("\"");

    if (len > 0) {
      struct_addr += len;
      // struct_addr = align_up((uint64_t)struct_addr, 4);
    }
  }

  print_s("\n");

  if (len > 0) {
    // struct_addr += len;
    // struct_addr = align_up((uint64_t)struct_addr, 4);
  }
  struct_addr = align_up((uint64_t)struct_addr, 4);
  return struct_addr;
}

uint32_t dtb_read_int(uint64_t *addr_ptr) {
  char *base = (char *)(*addr_ptr);
  uint32_t value = 0;
  for (int i = 3; i >= 0; i--) {
    value = (value << 8) | (*(base + i) & 0xFF);
  }
  *addr_ptr += 4;
  return value;
}

char *dtb_read_string(uint64_t *addr_ptr) {
  char *base = (char *)(*addr_ptr);
  char *string_start = base;
  while (*base) base++;
  base++;
  *addr_ptr = (uint64_t)base;
  return string_start;
}
