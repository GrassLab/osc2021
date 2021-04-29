#include "trap.h"

#include "exc.h"
#include "io.h"
#include "sched.h"
#include "syscall.h"
#include "timer.h"

#define CORE_INT_SRC_0 ((volatile unsigned int *)0x40000060)
#define CNTPNSIRQ_INT (1 << 1)

#define EC_MASK (0b111111 << 26)
#define SVC_AARCH64 (0b010101 << 26)

void exc_not_imp() {
  log("exception not implemented\n", LOG_ERROR);
  die();
}

void syn_handler(unsigned long el1_esr, void *el1_elr, void *el0_sp,
                 unsigned long el1_spsr, saved_reg *el1_sp) {
  unsigned long prev_int_stat = get_int_stat();
  set_int_stat(1);
  exc_lvl_gain();
  enable_interrupt();
  log("SYN\n", LOG_DEBUG);
  switch (el1_esr & EC_MASK) {
    case SVC_AARCH64:
      log_hex("svc", el1_esr & SVC_MASK, LOG_DEBUG);
      switch (el1_esr & SVC_MASK) {
        case SYS_EXIT:
          die();
          break;
        default:
          break;
      }
      break;
    default:
      log_hex("SYN NI\n", (unsigned long)el1_elr, LOG_ERROR);
      die();
      break;
  }
  disable_interrupt();
  exc_lvl_consume();
  set_int_stat(prev_int_stat);
  ret_exc(el1_elr, el0_sp, el1_spsr, (void *)el1_sp);
}

void irq_handler(void *el1_elr, void *el0_sp, unsigned long el1_spsr,
                 saved_reg *el1_sp) {
  set_int_stat(1);
  exc_lvl_gain();
  log("IRQ\n", LOG_DEBUG);
  if ((*CORE_INT_SRC_0 & CNTPNSIRQ_INT) != 0) {
    core_timer_handler();
  } else if ((*IRQ_PENDING_1 & (1 << 29)) != 0) {
    uart_handler();
  } else {
    log("IRQ not implement\n", LOG_ERROR);
  }
  exc_lvl_consume();
  set_int_stat(0);
  ret_exc(el1_elr, el0_sp, el1_spsr, (void *)el1_sp);
}