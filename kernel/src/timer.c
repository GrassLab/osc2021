#include "timer.h"

#include "io.h"
#include "utils.h"

void core_timer_enable() {
  asm volatile("mov x0, 1");
  asm volatile("msr cntp_ctl_el0, x0"); // enable
  asm volatile("mrs x0, cntfrq_el0");
  asm volatile("msr cntp_tval_el0, x0"); // set expired time
  asm volatile("mov x0, 2");
  asm volatile("ldr x1, =0x40000040");
  asm volatile("str w0, [x1]"); // unmask timer interrupt
}

void core_timer_handler() {
  uint64_t cntpct_el0, cntfrq_el0;
  asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct_el0));
  asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
  asm volatile("mrs x0, cntfrq_el0");
  asm volatile("mov x1, 2");
  asm volatile("mul x0, x0, x1");
  asm volatile("msr cntp_tval_el0, x0");
  print_s("Time elapsed after booting: ");
  print_i(cntpct_el0 / cntfrq_el0);
  print_s("s\n");
}
