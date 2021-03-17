#include "include/uart.h"
#include "include/reset.h"

extern char _kernel;
const char info[] = "Trying to load kernel.img from UART...";

void load_kernel() {
  unsigned int size;
  puts_uart(info);
  read_uart((char *)&size, 4);
  write_uart("Kernel base: ", 13);
  write_num_uart((unsigned long)&_kernel);
  write_uart("\n\r", 2);
  write_uart("Kernel size: ", 13);
  write_num_uart(size);
  write_uart("\n\r", 2);
  read_uart(&_kernel, size);
  write_uart("Start kernel", 13);

  ((void (*)())&_kernel)();
}

int main() {
  mini_uart_init();
  load_kernel();
  return 0;
}
