#include "uart.h"

void dumpState() {
  unsigned long esr, elr, spsr;
  asm volatile("mrs %0, esr_el1 \n" : "=r"(esr) :);
  asm volatile("mrs %0, elr_el1 \n" : "=r"(elr) :);
  asm volatile("mrs %0, spsr_el1    \n" : "=r"(spsr) :);

  uart_printf("--------------------\n");
  uart_printf("SPSR: %x\n", spsr);
  uart_printf("ELR: %x\n", elr);
  uart_printf("ESR: %x\n", esr);
  uart_printf("--------------------\n");
}

void syn_handler() {
  uart_println("Syn Exception");
  dumpState();
}

void irq_handler() {
  asm volatile("mrs x0, cntfrq_el0    \n");
  asm volatile("add x0, x0, x0        \n");
  asm volatile("msr cntp_tval_el0, x0 \n");
  unsigned long cntpct, cntfrq, tmp;
  asm volatile("mrs %0, cntpct_el0    \n" : "=r"(cntpct) :);
  asm volatile("mrs %0, cntfrq_el0    \n" : "=r"(cntfrq) :);

  tmp = cntpct * 10 / cntfrq;
  uart_printf("--------------------\n");
  uart_printf("Time Elapsed: %d.%ds\n", tmp / 10, tmp % 10);
  uart_printf("--------------------\n");
  while (1) {
  }
}

void _handler_not_impl() {
  uart_println("Unknown Exception!!");
  dumpState();
  while (1) {
  }
}
