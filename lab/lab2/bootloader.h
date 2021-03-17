#include "uart.h"
#include "utils.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))
#define SHIFT_ADDR 0x100000
#define KERNEL_ADDR 0x4000000

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