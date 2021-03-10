#include "include/uart.h"
#include "include/reset.h"
#include "include/string.h"
#include "include/cpio.h"

extern char _cpio_buf[];

char buffer[0x100];
const char hello[] = "Hello world!";
const char * const commands[] = {
  "hello",
  "cpio",
  "reboot",
  "help"
};

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
