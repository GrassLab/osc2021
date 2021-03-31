#include "include/uart.h"
#include "include/reset.h"
#include "include/string.h"
#include "include/cpio.h"
#include "include/dtb.h"
#include "include/mm.h"

extern void *_dtb_ptr;
char *initrd_ptr;
dtb_node *dtb;
int indent = 0;
char buffer[0x100];

const char hello[] = "Hello world!";
const char * const commands[] = {
  "hello",
  "cpio",
  "print-dtb",
  "reboot",
  "help"
};

static uint32_t get_be_int(const void *ptr) {
    return __builtin_bswap32(*(uint32_t *)ptr);
}

uint64_t get_be_long(const void *ptr) {
    return __builtin_bswap64(*(uint64_t *)ptr);
}

void shell() {
  for (;;) {
    write_uart("> ", 2);
    interact_readline_uart(buffer);
    if (!strcmp("help", buffer)) {
      puts_uart("Commands:");
      for (int i = 0; i < sizeof(commands)/sizeof(void *); i++) {
        puts_uart(commands[i]);
      }

    } else if (!strcmp("hello", buffer)) {
      puts_uart(hello);

    } else if (!strncmp("cpio", buffer, 4) && initrd_ptr) {
      if (strlen(buffer) < 6) {
        cpio_list_file(initrd_ptr);
      } else {
        const char *content = cpio_get_content(&buffer[5], initrd_ptr);
        if (content) {
          puts_uart(content);
        } else {
          puts_uart("file not found.");
        }
      }

    } else if (!strcmp("print-dtb", buffer) && _dtb_ptr) {
      if (_dtb_ptr) {
        print_device_tree(dtb);
      }

    } else if (!strcmp("reboot", buffer)) {
      puts_uart("reboot machine");
      reset(100);
    }
  }
}

int main() {
  if (_dtb_ptr) {
    dtb = build_device_tree(_dtb_ptr);
    dtb_prop *prop = find_device_tree(dtb, "/chosen/linux,initrd-start");
    if (prop) {
      initrd_ptr = (char *)(uintptr_t)get_be_int(prop->data);
    }
  }
  
  shell();
  return 0;
}
