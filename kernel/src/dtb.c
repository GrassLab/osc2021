#include "dtb.h"

#include "io.h"
#include "string.h"
#include "utils.h"

void mailbox_probe(uint64_t struct_addr, uint64_t strings_addr, int depth) {
  if (check_compatibility(struct_addr, strings_addr, "brcm,bcm2835-mbox") == 0)
    return;
  print_node(struct_addr, strings_addr, depth);
}

void gpio_probe(uint64_t struct_addr, uint64_t strings_addr, int depth) {
  if (check_compatibility(struct_addr, strings_addr, "brcm,bcm2835-gpio") == 0)
    return;
  print_node(struct_addr, strings_addr, depth);
}

void rtx3080ti_probe(uint64_t struct_addr, uint64_t strings_addr, int depth) {
  if (check_compatibility(struct_addr, strings_addr, "rtx3080ti") == 0) return;
  print_node(struct_addr, strings_addr, depth);
}

void default_probe(uint64_t struct_addr, uint64_t strings_addr, int depth) {
  print_node(struct_addr, strings_addr, depth);
}

int check_compatibility(uint64_t struct_addr, uint64_t strings_addr,
                        char *compatible_name) {
  char *name = (char *)(struct_addr);
  struct_addr += strlen(name) + 1;
  struct_addr = align_up(struct_addr, 4);

  while (1) {
    uint32_t token = dtb_read_int(struct_addr);
    token = be2le(token);
    struct_addr += 4;

    if (token == FDT_PROP) {
      uint32_t len = dtb_read_int(struct_addr);
      len = be2le(len);
      struct_addr += 4;
      uint32_t nameoff = dtb_read_int(struct_addr);
      nameoff = be2le(nameoff);
      struct_addr += 4;
      char *property = (char *)(strings_addr + nameoff);

      if (strcmp(property, "compatible") == 0) {
        uint64_t end = struct_addr + len;
        while (struct_addr < end) {
          char *string = (char *)struct_addr;
          struct_addr += strlen(string) + 1;
          if (strcmp(string, compatible_name) == 0) return 1;
        }
        return 0;
      } else {
        struct_addr += len;
        struct_addr = align_up(struct_addr, 4);
      }
    } else if (token == FDT_NOP) {
      continue;
    } else {
      break;
    }
  }
  return 0;
}

void dtb_print(int all) {
  uint64_t dtb_addr = *((uint64_t *)0x9000000);
  fdt_header *header = (fdt_header *)dtb_addr;
  uint64_t struct_addr = dtb_addr + be2le(header->off_dt_struct);
  uint64_t strings_addr = dtb_addr + be2le(header->off_dt_strings);

  if (all) {
    dtb_parse(struct_addr, strings_addr, default_probe);
  } else {
    print_s("\ndevice for mailbox driver function: \n");
    dtb_parse(struct_addr, strings_addr, mailbox_probe);
    print_s("\ndevice for gpio driver function: \n");
    dtb_parse(struct_addr, strings_addr, gpio_probe);
    print_s("\ndevice for rtx3080ti driver function: \n");
    dtb_parse(struct_addr, strings_addr, rtx3080ti_probe);
  }
}

void dtb_parse(uint64_t struct_addr, uint64_t strings_addr,
               void (*callback)(uint64_t, uint64_t, int)) {
  int depth = 0;
  while (1) {
    uint32_t token = dtb_read_int(struct_addr);
    token = be2le(token);
    struct_addr += 4;

    if (token == FDT_BEGIN_NODE) {
      callback(struct_addr, strings_addr, depth);
      struct_addr = ignore_current_node(struct_addr, strings_addr);
      depth++;
    } else if (token == FDT_END_NODE) {
      depth--;
    } else if (token == FDT_NOP) {
      continue;
    } else if (token == FDT_END) {
      break;
    }
  }
}

uint64_t ignore_current_node(uint64_t struct_addr, uint64_t strings_addr) {
  char *name = (char *)(struct_addr);
  struct_addr += strlen(name) + 1;
  struct_addr = align_up(struct_addr, 4);

  while (1) {
    uint32_t token = dtb_read_int(struct_addr);
    token = be2le(token);
    struct_addr += 4;

    if (token == FDT_BEGIN_NODE) {
      struct_addr -= 4;
      break;
    } else if (token == FDT_END_NODE) {
      struct_addr -= 4;
      break;
    } else if (token == FDT_PROP) {
      uint32_t len = dtb_read_int(struct_addr);
      len = be2le(len);
      struct_addr += 4;
      uint32_t nameoff = dtb_read_int(struct_addr);
      nameoff = be2le(nameoff);
      struct_addr += 4;
      struct_addr += len;
      struct_addr = align_up(struct_addr, 4);
    } else if (token == FDT_NOP) {
      continue;
    }
  }
  return struct_addr;
}

uint64_t print_node(uint64_t struct_addr, uint64_t strings_addr, int depth) {
  char *name = (char *)(struct_addr);
  struct_addr += strlen(name) + 1;
  struct_addr = align_up(struct_addr, 4);
  for (int i = 0; i < depth; i++) print_s("    ");
  print_s("node: ");
  print_s(name);
  print_s("\n");

  while (1) {
    uint32_t token = dtb_read_int(struct_addr);
    token = be2le(token);
    struct_addr += 4;

    if (token == FDT_BEGIN_NODE) {
      break;
    } else if (token == FDT_END_NODE) {
      struct_addr -= 4;
      break;
    } else if (token == FDT_PROP) {
      struct_addr = print_property(struct_addr, strings_addr, depth + 1);
    } else if (token == FDT_NOP) {
      continue;
    }
  }
  return struct_addr;
}

uint64_t print_property(uint64_t struct_addr, uint64_t strings_addr,
                        int depth) {
  uint32_t len = dtb_read_int(struct_addr);
  len = be2le(len);
  struct_addr += 4;
  uint32_t nameoff = dtb_read_int(struct_addr);
  nameoff = be2le(nameoff);
  struct_addr += 4;
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

  // hex, e.g. <0x4600 0x100>
  if (value_type == 0) {
    int count = 0;
    print_s("<");
    for (uint32_t i = 0; i < len; i += 4) {
      if (count > 0) print_s(" ");
      count++;
      uint32_t num = dtb_read_int(struct_addr);
      num = be2le(num);
      struct_addr += 4;
      print_h(num);
    }
    print_s(">");
  }
  // int, e.g. <1>
  if (value_type == 1) {
    uint32_t num = dtb_read_int(struct_addr);
    num = be2le(num);
    struct_addr += 4;
    print_s("<");
    print_i(num);
    print_s(">");
  }
  // string or string list, e.g. "fsl,MPC8349EMITX" or "fsl,mpc8641", "ns16550"
  if (value_type == 2) {
    int count = 0;
    uint64_t end = struct_addr + len;

    while (struct_addr < end) {
      if (count > 0) print_s(", ");
      count++;
      char *string = (char *)struct_addr;
      struct_addr += strlen(string) + 1;
      print_s("\"");
      print_s(string);
      print_s("\"");
    }
  }
  print_s("\n");

  struct_addr = align_up(struct_addr, 4);
  return struct_addr;
}

uint32_t dtb_read_int(uint64_t addr_ptr) {
  char *base = (char *)(addr_ptr);
  uint32_t value = 0;
  for (int i = 3; i >= 0; i--) {
    value = (value << 8) | (*(base + i) & 0xFF);
  }
  return value;
}

// char *dtb_read_string(uint64_t addr_ptr) {
//   char *base = (char *)(addr_ptr);
//   char *string_start = base;
//   return string_start;
// }
