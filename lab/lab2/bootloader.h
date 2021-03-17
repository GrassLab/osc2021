#include "uart.h"
#include "utils.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))
#define SHIFT_ADDR 0x100000
#define KERNEL_ADDR 0x4000000
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

void dtb_valueTypePrint(char *name, char **fp, int len) {
  uart_puts(name);
  uart_puts(": ");
  if (strcmp("compatible", name) || strcmp("model", name) ||
      strcmp("status", name) || strcmp("name", name) ||
      strcmp("device_type", name)) {
    for (int i = 0; i < len; i++) uart_send(get_char8(fp));
    uart_puts("\r\n");
  } else if (strcmp("phandle", name) || strcmp("#address-cells", name) ||
             strcmp("#size-cells", name) || strcmp("virtual-reg", name) ||
             strcmp("interrupt-parent", name)) {
    print_h(big2little(get_int32((unsigned int **)fp)));
  } else {
    if (len == 0) uart_puts("N / A");
    for (int i = 0; i < len; i++) {
      char c = get_char8(fp);
      print_hc(c);
      if (i < len - 1) uart_puts(":");
    }
    uart_puts("\r\n");
  }
}

void test_probe(char *compatible) {
  int k = 0;
  while (compatible[k] != '\0') k++;
  compatible = &compatible[k + 1];
  /* test led*/
  struct fdt_header *dts_header = *(struct fdt_header **)_save_dts;
  /* shift to struct offset*/
  char *fp = (char *)dts_header + big2little(dts_header->off_dt_struct),
       name[buff_size];
  int deep = -1;
  int bool = 0;
  while (1) {
    int FDT_NODE = big2little(get_int32((unsigned int **)&fp));

    if (FDT_NODE == FDT_NODE_BEGIN) {
      char c;
      int i;
      for (i = 1; (c = get_char8(&fp)) != '\0'; i++) name[i - 1] = c;
      name[i - 1] = '\0';
      fp += align(i, 4);
      deep++;
    } else if (FDT_NODE == FDT_NODE_END) {
      bool = 0;
      deep--;
    } else if (FDT_NODE == FDT_PROP) {
      /* FDT_PROP struct */
      int len = big2little(get_int32((unsigned int **)&fp));
      int nameoff = big2little(get_int32((unsigned int **)&fp));
      char *type =
          (char *)dts_header + big2little(dts_header->off_dt_strings) + nameoff;
      if (strcmp("compatible", type)) {
        char *value = fp;
        fp += len;
        while (value < fp) {
          if (find(compatible, value) > -1) {
            uart_puts("name: ");
            uart_puts(name);
            uart_puts("\r\n");
            uart_puts("compatible: ");
            uart_puts(value);
            uart_puts("\r\n");
            bool = 1;
            break;
          } else
            value += strlen(value) + 1;
        }

      } else if (bool)
        dtb_valueTypePrint((char *)dts_header +
                               big2little(dts_header->off_dt_strings) + nameoff,
                           &fp, len);
      else
        fp += len;
      fp += align(len, 4);
    } else if (FDT_NODE == FDT_NOP)
      continue;
    else if (FDT_NODE == FDT_END)
      break;
  }
}