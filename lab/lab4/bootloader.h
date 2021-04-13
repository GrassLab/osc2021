#ifndef _BOOT
#define _BOOT
#include "uart.h"
#include "utils.h"

#define SHIFT_ADDR 0x400000
#define KERNEL_ADDR 0x80000

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))

#define FDT_NODE_BEGIN 0x00000001
#define FDT_NOP 0x00000004
#define FDT_NODE_END 0x00000002
#define FDT_END 0x00000009
#define FDT_PROP 0x00000003

extern char _end[];
extern char _start[];
extern char _save_dts[];

struct fdt_header {
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
};

void dtb_printNode(char *name, char *value, int len, char *type, int FDT_NODE,
                   int deep) {
  if (FDT_NODE == FDT_NODE_BEGIN) {
    uart_puts("name: ");
    uart_puts(name);
    uart_puts("\r\n");
  } else if (FDT_NODE == FDT_PROP) {
    uart_puts(type);
    uart_puts(": ");
    if (strcmp("compatible", type) || strcmp("model", type) ||
        strcmp("status", type) || strcmp("name", type) ||
        strcmp("device_type", type)) {
      for (int i = 0; i < len; i++) uart_send(value[i]);
      uart_puts("\r\n");
    } else if (strcmp("phandle", type) || strcmp("#address-cells", type) ||
               strcmp("#size-cells", type) || strcmp("virtual-reg", type) ||
               strcmp("interrupt-parent", type)) {
      print_h(big2little(get_int32((unsigned int **)&value)));
      uart_puts("\r\n");
    } else {
      if (len == 0) uart_puts("N/A");
      for (int i = 0; i < len; i++) {
        print_hc(get_char8(&value));
        if (i < len - 1) uart_puts(":");
      }
      uart_puts("\r\n");
    }
  }
}

int dtb_getNode(char **fp, char **name, char **value, int *len, char **type,
                int *FDT_NODE, struct fdt_header *header, int deep) {
  *FDT_NODE = big2little(get_int32((unsigned int **)fp));
  if (*FDT_NODE == FDT_NODE_BEGIN) {
    *name = *fp;
    *fp += strlen(*name) + 1 + align(strlen(*name) + 1, 4);
    return ++deep;
  } else if (*FDT_NODE == FDT_PROP) {
    *len = big2little(get_int32((unsigned int **)fp));
    *type = (char *)header + big2little(header->off_dt_strings) +
            big2little(get_int32((unsigned int **)fp));
    *value = *fp;
    uart_puts("");  // unknown bug, if remove *fp add result will error
    (*fp) += (align(*len, 4) + (*len));
  } else if (*FDT_NODE == FDT_NODE_END)
    return --deep;
  else if (*FDT_NODE == FDT_NOP)
    ;
  else if (*FDT_NODE == FDT_END)
    return 0;
  return deep;
}

#endif /*_BOOT */
