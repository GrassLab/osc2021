#include "exc.h"
#include "io.h"
#include "syscall.h"
#include "timer.h"

#define CORE_INT_SRC_0 ((volatile unsigned int *)0x40000060)
#define CNTPNSIRQ_INT (1 << 1)

typedef struct saved_reg {
  unsigned long x30, pad, x28, x29, x26, x27, x24, x25, x22, x23, x20, x21;
  unsigned long x18, x19, x16, x17, x14, x15, x12, x13, x10, x11;
  unsigned long x8, x9, x6, x7, x4, x5, x2, x3, x0, x1;
} saved_reg;

void exc_not_imp() { log("exception not implemented\n", LOG_ERROR); }

void syn_handler(unsigned long el1_esr, void *el1_elr, void *el0_sp,
                 unsigned long el1_spsr, saved_reg *el1_sp) {
  // get_interrupt();
  log("SYN\n", LOG_DEBUG);
  switch (el1_esr) {
    case SVC_MASK | SYS_EXIT:
      ret_kern(el1_sp);
      break;
    default:
      break;
  }
}

void irq_handler(void *el1_elr, void *el0_sp, unsigned long el1_spsr,
                 saved_reg *el1_sp) {
  get_interrupt();
  log("IRQ\n", LOG_DEBUG);
  if ((*CORE_INT_SRC_0 & CNTPNSIRQ_INT) != 0) {
    core_timer_handler();
  } else if((*IRQ_PENDING_1 & (1 << 29)) != 0) {
    uart_handler();
  }
  ret_interrupt();
  switch_exec(el1_elr, el0_sp, el1_spsr, (void *)el1_sp);
}