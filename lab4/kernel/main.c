#include "uart.h"
#include "reset.h"
#include "string.h"
#include "cpio.h"
#include "dtb.h"
#include "mm.h"
#include "printf.h"
#include "exec.h"
#include "timer.h"

char *initrd_ptr, *initrd_end;
dtb_node *dtb;
char buffer[0x100];

const char * const commands[] = {
  "hello",
  "cpio",
  "exec",
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
    printf("> ");
    interact_readline_uart(buffer);
    if (!strcmp("help", buffer)) {
      puts("Commands:");
      for (int i = 0; i < sizeof(commands)/sizeof(void *); i++) {
        puts(commands[i]);
      }

    } else if (!strcmp("hello", buffer)) {
      printf("Hello world!");

    } else if (!strncmp("cpio", buffer, 4) && initrd_ptr) {
      if (strlen(buffer) < 6) {
        cpio_list_file(initrd_ptr);
      } else {
        struct file *content = cpio_get_file(&buffer[5], initrd_ptr, initrd_end);
        if (content) {
          puts(content->data);
        } else {
          puts("file not found.");
        }
      }

    } else if (!strncmp("exec", buffer, 4) && initrd_ptr) {
      if (strlen(buffer) >= 6) {
        struct file *f = cpio_get_file(&buffer[5], initrd_ptr, initrd_end);
        if (f)
          exec_binary(f);

        /* exec failed */
        puts("exec failed");
      }

    } else if (!strcmp("print-dtb", buffer)) {
      if (dtb) {
        print_device_tree(dtb);
      }

    } else if (!strcmp("reboot", buffer)) {
      puts("reboot machine");
      reset(100);
    }
  }
}

int main(void *_dtb_ptr) {
  if (_dtb_ptr) {
    dtb = build_device_tree(_dtb_ptr);
    dtb_prop *prop = find_device_tree(dtb, "/chosen/linux,initrd-start");
    if (prop) {
      initrd_ptr = (char *)(uintptr_t)get_be_int(prop->data);
      printf("[Kernel] initrd: 0x%p\n\r", initrd_ptr);
    }
    prop = find_device_tree(dtb, "/chosen/linux,initrd-end");
    if (prop) {
      initrd_end = (char *)(uintptr_t)get_be_int(prop->data);
    }
  }

  enable_core_timer();
  shell();
  return 0;
}
