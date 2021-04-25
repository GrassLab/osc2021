#include "exception.h"
#include "syscall.h"
#include "timer.h"
#include "uart.h"

#include "stddef.h"
#include "stdint.h"

#define EC_SVC_AARCH64 0b010101

// Get field inside an int
// example: EC is at bit 29-24 in variable "ELR"
//  ->  get_bits(ELR, 24, 6)
#define get_bits(var, base_shift, mask_len)                                    \
  (((var) >> (base_shift)) & (((1 << (mask_len)) - 1)))

struct Exception {
  uint8_t ec;        // Exception class
  uint32_t iss;      // Instruction specific syndrome
  uint64_t ret_addr; // Exception return address
};

void dumpState() {
  unsigned long esr, elr, spsr;
  asm volatile("mrs %0, esr_el1 \n" : "=r"(esr) :);
  asm volatile("mrs %0, elr_el1 \n" : "=r"(elr) :);
  asm volatile("mrs %0, spsr_el1    \n" : "=r"(spsr) :);

  uart_println("--------------------");
  uart_println("SPSR: %x, ELR:%x, ESR: %x", spsr, elr, esr);
}

void syn_handler(struct trap_frame *tf) {
  uart_println("Syn Exception");
  struct Exception exception;
  uint32_t esr_el1;

  asm volatile("mrs %0, elr_el1 \n" : "=r"(exception.ret_addr) :);
  asm volatile("mrs %0, esr_el1 \n" : "=r"(esr_el1) :);
  dumpState();
  exception.ec = get_bits(esr_el1, 26, 6);
  exception.iss = get_bits(esr_el1, 0, 25);

  switch (exception.ec) {

  // cheetsheet for syscall in this OS
  // syscall_NR, return, arg0, arg1, arg2, arg3
  //         x8,     x0,   x0,   x1,   x2,   x3
  case EC_SVC_AARCH64:
    syscall_routing(tf->regs[8], tf);
    break;
  default:
    uart_println("Unknown exception, ec:%d iss:%d", exception.ec,
                 exception.iss);
  }
}

void irq_handler() {
  unsigned long cntpct = timer_el0_get_cnt();
  unsigned long cntfrq = timer_el0_get_freq();
  unsigned long tmp = cntpct * 10 / cntfrq;
  uart_println("--------------------");
  uart_println("Time Elapsed: %d.%ds", tmp / 10, tmp % 10);
  uart_println("--------------------");
  timer_el0_set_timeout();
}

void _handler_not_impl() {
  uart_println("Unknown Exception!!");
  dumpState();
  while (1) {
  }
}
