#include <printf.h>
#include <uart.h>
#include <reset.h>

extern char _kernel[];
extern char *_dtb;

void load_kernel() {
  unsigned int size;
  puts("Trying to load kernel.img from UART...");

  read_uart((char *)&size, 4);
  printf("Kernel base: %p\n\r", _kernel);
  printf("Kernel size: %d\n\r", size);

  read_uart(_kernel, size);
  puts("Start kernel");

  ((void (*)(char *))_kernel)(_dtb);
}

int main() {
  mini_uart_init();
  load_kernel();
  return 0;
}
