#include "include/uart.h"
#include "include/reset.h"
#include "include/string.h"
#include "include/cpio.h"
#include "include/dtb.h"

extern char _cpio_buf[];
extern void *_dtb_ptr;

char buffer[0x100];
const char hello[] = "Hello world!";
const char * const commands[] = {
  "hello",
  "cpio",
  "parse-dtb",
  "reboot",
  "help"
};

int indent = 0;

void write_indent(int n) {
  while (n--) write_uart(" ", 1);
}

void callback(int type, const char *name, const void *data, uint32_t size) {
  switch(type) {
    case FDT_BEGIN_NODE:
      write_indent(indent);
      print_uart(name);
      puts_uart("{");
      indent++;
      break;
      
    case FDT_END_NODE:
      indent--;
      write_indent(indent);
      puts_uart("}");
      break;

    case FDT_NOP:
      break;

    case FDT_PROP:
      write_indent(indent);
      puts_uart(name);
      break;

    case FDT_END:
      break;
  }
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

    } else if (!strncmp("cpio", buffer, 4)) {
      if (strlen(buffer) < 6) {
        cpio_list_file(_cpio_buf);
      } else {
        const char *content = cpio_get_content(&buffer[5], _cpio_buf);
        if (content) {
          puts_uart(content);
        } else {
          puts_uart("file not found.");
        }
      }

    } else if (!strcmp("parse-dtb", buffer)) {
      if (_dtb_ptr)
        traverse_device_tree(_dtb_ptr, callback);

    } else if (!strcmp("reboot", buffer)) {
      puts_uart("reboot machine");
      reset(100);
    }
  }
}

int main() {
  shell();
  return 0;
}
