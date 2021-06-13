#include "exception.h"
#include "syscall.h"
#include "timer.h"
#include "uart.h"

#include "stddef.h"
#include "stdint.h"

#define EC_SVC_AARCH64 0b010101
#define EC_SVC_DATA_ABORT 0b100101
#define EC_SP_ALIGNMENT_FAULT 0b100100

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
  uint64_t esr, elr, spsr;
  asm volatile("mrs %0, esr_el1 \n" : "=r"(esr) :);
  asm volatile("mrs %0, elr_el1 \n" : "=r"(elr) :);
  asm volatile("mrs %0, spsr_el1    \n" : "=r"(spsr) :);

  uart_println("--------------------");
  uart_println("SPSR: %x, ELR:%x, ESR: %x", spsr, elr, esr);
}

void syn_handler(struct trap_frame *tf) {
  struct Exception exception;
  uint32_t esr_el1;

  asm volatile("mrs %0, elr_el1 \n" : "=r"(exception.ret_addr) :);
  asm volatile("mrs %0, esr_el1 \n" : "=r"(esr_el1) :);
  exception.ec = get_bits(esr_el1, 26, 6);
  exception.iss = get_bits(esr_el1, 0, 25);

  switch (exception.ec) {

  // cheetsheet for syscall in this OS
  // syscall_NR, return, arg0, arg1, arg2, arg3
  //         x8,     x0,   x0,   x1,   x2,   x3
  case EC_SVC_AARCH64:
    syscall_routing(tf->regs[8], tf);
    break;
  case EC_SVC_DATA_ABORT:
    dumpState();
    uart_println("Data abort taken, ec:%x iss:%x", exception.ec, exception.iss);
    while (1) {
      ;
    }
  case EC_SP_ALIGNMENT_FAULT:
    dumpState();
    uint64_t sp_el0;
    asm volatile("mrs %0, sp_el0    \n" : "=r"(sp_el0));

    uart_println("SP is not aligned, ec:%x iss:%x", exception.ec,
                 exception.iss);
    uart_println("sp_el0: %x", sp_el0);
    while (1) {
      ;
    }
  default:
    dumpState();
    uart_println("Unknown exception, ec:%x iss:%x", exception.ec,
                 exception.iss);
    while (1) {
      ;
    }
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
