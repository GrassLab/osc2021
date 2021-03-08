#include "uart.h"
#include "reset.h"

char buffer[0x100];
const char hello[] = "Hello world!";
const char * const commands[] = {
  "hello",
  "reboot",
  "help"
};


int strcmp(const char *a, const char *b) {
  int i = 0;
  while (a[i] == b[i] && a[i] != '\0' && b[i] != '\0') i++;
  return a[i] == b[i];
}

void shell() {
  for (;;) {
    write_uart("> ", 2);
    interact_readline_uart(buffer);
    if (strcmp("help", buffer)) {
      puts_uart("Commands:");
      for (int i = 0; i < sizeof(commands)/sizeof(void *); i++) {
        puts_uart(commands[i]);
      }
    } else if (strcmp("hello", buffer)) {
      puts_uart(hello);
    } else if (strcmp("reboot", buffer)) {
      reset(10);
      puts_uart("reboot machine");
    }
  }
}

int main() {
  shell();
  return 0;
}
