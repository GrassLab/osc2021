#include "shell.h"

#include "io.h"
#include "mini_uart.h"
#include "string.h"

void cmd_help() {
  print_s("Command\t\tDescription\n");
  print_s("---------------------------------------------\n");
  print_s("help\t\tprint all available commands\n");
  print_s("hello\t\tprint Hello World!\n");
  print_s("reboot\t\treboot machine\n");
  print_s("loadimg\t\tload image through UART\n");
}

void cmd_hello() { print_s("Hello World!\n"); }

void cmd_reboot(int tick) {       // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20;  // full reset
  *PM_WDOG = PM_PASSWORD | tick;  // number of watchdog tick
}

void cmd_loadimg() {
  // relocate bootloader
  extern void *_start;
  extern void *_end;
  unsigned long long bootloader_size =
      (unsigned long long)&_end - (unsigned long long)&_start;

  char *original_addr = (char *)&_start;
  char *relocated_addr = (char *)0x1000000;

  for (unsigned long long i = 0; i < bootloader_size; i++) {
    *(relocated_addr + i) = *(original_addr + i);
  }
  print_s("relocate bootloader done\n");

  // jump to read_image() in relocated bootloader
  unsigned long long relocated_read_image =
      (unsigned long long)relocated_addr +
      ((unsigned long long)&read_image - (unsigned long)original_addr);
  ((void (*)())relocated_read_image)();
}

void read_image() {
  print_s("Please send the image with UART.\r\n");

  // read image
  int img_size = read_i();
  char *base_addr = (char *)(0x80000);
  for (int i = 0; i < img_size; i++) {
    *(base_addr + i) = read_b();
  }
  print_s("read kernel image done\n");

  // jump to kernel
  ((void (*)())(long long int)base_addr)();
}

void clear_buffer() {
  buffer_pos = 0;
  for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
    buffer[i] = '\0';
  }
}

void receive_cmd() {
  while (1) {
    char c = uart_getc();
    if (c == '\0') continue;  // to avoid weird character
    if (c == '\n') {          // '\r' is replaced with '\n'
      print_s("\r\n");
      buffer[buffer_pos] = '\0';
      break;
    }
    print_c(c);
    buffer[buffer_pos++] = c;
  }
}

void run_shell() {
  print_s("**************************\n");
  print_s("** 3rd stage bootloader **\n");
  print_s("**************************\n");
  while (1) {
    print_s("% ");
    clear_buffer();
    receive_cmd();
    if (strcmp(buffer, "help") == 0) cmd_help();
    if (strcmp(buffer, "hello") == 0) cmd_hello();
    if (strcmp(buffer, "reboot") == 0) {
      cmd_reboot(100);
      break;
    }
    if (strcmp(buffer, "loadimg") == 0) cmd_loadimg();
  }
}
