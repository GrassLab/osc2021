#include "exception.h"

#include "mini_uart.h"
#include "printf.h"
#include "syscall.h"
#include "thread.h"
#include "timer.h"

int count = 0;

void enable_interrupt() { asm volatile("msr DAIFClr, 0xf"); }

void disable_interrupt() { asm volatile("msr DAIFSet, 0xf"); }

void sync_handler_currentEL_ELx() {
  uint64_t spsr_el1, elr_el1, esr_el1;
  asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
  asm volatile("mrs %0, elr_el1" : "=r"(elr_el1));
  asm volatile("mrs %0, esr_el1" : "=r"(esr_el1));
  printf("SPSR_EL1: 0x%08x\n", spsr_el1);
  printf("ELR_EL1: 0x%08x\n", elr_el1);
  printf("ESR_EL1: 0x%08x\n", esr_el1);
}

void sync_handler_lowerEL_64(uint64_t sp) {
  uint64_t spsr_el1, elr_el1, esr_el1;
  asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
  asm volatile("mrs %0, elr_el1" : "=r"(elr_el1));
  asm volatile("mrs %0, esr_el1" : "=r"(esr_el1));
  // printf("sync, SPSR_EL1: 0x%08x\n", spsr_el1);
  // printf("ELR_EL1: 0x%08x\n", elr_el1);
  // printf("ESR_EL1: 0x%08x\n", esr_el1);

  uint32_t ec = (esr_el1 >> 26) & 0x3f;
  // printf("EC: %x\n", ec);
  if (ec == 0b010101) {  // SVC instruction
    uint32_t iss = esr_el1 & ((1 << 25) - 1);
    // printf("syscall number: %d\n", iss);
    trap_frame_t *trap_frame = (trap_frame_t *)sp;
    syscall_handler(iss, trap_frame);
  } else if (ec == 0b100100) {  // SVC instruction
    uint64_t far;
    asm volatile("mrs %0, far_el1" : "=r"(far));
    printf("[Data Abort] pid: %d, far_el1: 0x%llx\n", get_current()->pid, far);
    exit();
  }
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

void default_handler() { printf("===== default handler =====\n"); }
