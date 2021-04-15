#include "timer.h"
#include "uart.h"
static inline void wait_slow_down() {
  for (long i = 0; i < 1000000; i++) {
    ;
  }
}
void dumpState() {
  unsigned long esr, elr, spsr;
  asm volatile("mrs %0, esr_el1 \n" : "=r"(esr) :);
  asm volatile("mrs %0, elr_el1 \n" : "=r"(elr) :);
  asm volatile("mrs %0, spsr_el1    \n" : "=r"(spsr) :);

  uart_println("--------------------");
  uart_println("SPSR: %x, ELR:%x, ESR: %x", spsr, elr, esr);
}

void syn_handler() {
  uart_println("Syn Exception");
  dumpState();
}

void irq_handler() {
  unsigned long cntpct = timer_el0_get_cnt();
  unsigned long cntfrq = timer_el0_get_freq();
  unsigned long tmp = cntpct * 10 / cntfrq;
  uart_println("--------------------");
  uart_println("Time Elapsed: %d.%ds", tmp / 10, tmp % 10);
  uart_println("--------------------");
  timer_el0_set_timeout();
  uart_println("Busy waiting for demo...");
  wait_slow_down();
}

void _handler_not_impl() {
  uart_println("Unknown Exception!!");
  dumpState();
  while (1) {
  }
}
