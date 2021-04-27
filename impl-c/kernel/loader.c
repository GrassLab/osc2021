#include "cpio.h"
#include "exec.h"
#include "mm.h"
#include "mm/frame.h"
#include "stddef.h"
#include "stdint.h"
#include "timer.h"
#include "uart.h"

// load program into a seperate memory space
void *load_program(const char *name) {
  unsigned long size;
  uint8_t *file = (uint8_t *)cpioGetFile((void *)RAMFS_ADDR, name, &size);
  if (file == NULL) {
    uart_println("[Loader]Cannot found `%s` under rootfs", name);
    return NULL;
  }
  unsigned char *load_addr = (unsigned char *)kalloc(FRAME_SIZE);
  for (unsigned long i = 0; i < size; i++) {
    load_addr[i] = file[i];
  }
  return load_addr;
}

// Run program without interacting with scheduling mechanism
void exec(const char *name, int argc, const char **argv) {
  void *load_addr = load_program(name);
  if (load_addr == NULL) {
    uart_println("exec failed");
    return;
  }

  unsigned char *stack_base = (unsigned char *)kalloc(FRAME_SIZE);
  uart_println("[exec] addr for user program: %x, size:%d", load_addr,
               FRAME_SIZE);
  asm volatile("mov x0, 0x340  \n"); // enable core timer interrupt
  asm volatile("msr spsr_el1, x0  \n");
  asm volatile("msr elr_el1, %0   \n" ::"r"(load_addr));
  asm volatile("msr sp_el0, %0    \n" ::"r"(stack_base + FRAME_SIZE));

  // enable the core timer’s interrupt in el0
  timer_el0_enable();
  timer_el0_set_timeout();

  // unmask timer interrupt
  asm volatile("mov x0, 2             \n");
  asm volatile("ldr x1, =0x40000040   \n");
  asm volatile("str w0, [x1]          \n");

  asm volatile("eret              \n");
}
