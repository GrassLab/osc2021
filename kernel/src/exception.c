#include "exception.h"

#include "io.h"
#include "mini_uart.h"
#include "timer.h"
#include "utils.h"

int count = 0;

void enable_interrupt() { asm volatile("msr DAIFClr, 0xf"); }

void disable_interrupt() { asm volatile("msr DAIFSet, 0xf"); }

void sync_handler() {
  uint64_t spsr_el1, elr_el1, esr_el1;
  asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
  asm volatile("mrs %0, elr_el1" : "=r"(elr_el1));
  asm volatile("mrs %0, esr_el1" : "=r"(esr_el1));
  print_s("SPSR_EL1: ");
  print_h(spsr_el1);
  print_s("\n");
  print_s("ELR_EL1: ");
  print_h(elr_el1);
  print_s("\n");
  print_s("ESR_EL1: ");
  print_h(esr_el1);
  print_s("\n");
}

void irq_handler_currentEL_ELx() {
  disable_interrupt();
  uint32_t is_uart =
      (*CORE0_IRQ_SOURCE & GPU_IRQ) && (*IRQ_PENDING_1 & AUX_IRQ);
  uint32_t is_core_timer = (*CORE0_IRQ_SOURCE & CNTPNS_IRQ);

  if (is_uart) {
    uart_handler();
  } else if (is_core_timer) {
    core_timer_handler_currentEL_ELx();
  }
  enable_interrupt();
}

void irq_handler_lowerEL_64() {
  disable_interrupt();
  uint32_t is_uart =
      (*CORE0_IRQ_SOURCE & GPU_IRQ) && (*IRQ_PENDING_1 & AUX_IRQ);
  uint32_t is_core_timer = (*CORE0_IRQ_SOURCE & CNTPNS_IRQ);

  if (is_uart) {
    uart_handler();
  } else if (is_core_timer) {
    core_timer_handler_lowerEL_64();
  }
  enable_interrupt();
}

void default_handler() { print_s("===== default handler =====\n"); }
