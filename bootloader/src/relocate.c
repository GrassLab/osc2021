#include "string.h"
#include "uart.h"

extern unsigned char _begin, _end, __relocate_target_addr;

__attribute__((section(".text.relocate"))) void relocate() {
  uart_init();

  uart_println("-------------------------------");
  uart_println(" Boot Loader");

  unsigned long kernel_size = (&_end - &_begin);
  uart_println(" Kernel relocation\r\n"
               "    Current kernel mem addr: %x - %x\n"
               "    Kernel size: %d bytes\n"
               "    Relocate to %x\n",
               &_begin, &_end, kernel_size, &__relocate_target_addr);

  memcpy(&__relocate_target_addr, &_begin, kernel_size);

  uart_println("    [Finished]");

  void (*start)(void) = (void *)&__relocate_target_addr;
  start();
}