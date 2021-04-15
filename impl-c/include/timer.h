#pragma once

static inline unsigned long timer_el0_get_freq() {
  unsigned long cntfrq;
  asm volatile("mrs %0, cntfrq_el0    \n" : "=r"(cntfrq) :);
  return cntfrq;
}

static inline unsigned long timer_el0_get_cnt() {
  unsigned long cntpct;
  asm volatile("mrs %0, cntpct_el0    \n" : "=r"(cntpct) :);
  return cntpct;
}

static inline void timer_el0_set_timeout() {
  asm volatile("mrs x0, cntfrq_el0    \n");
  asm volatile("add x0, x0, x0        \n"); // 2 secs
  asm volatile("msr cntp_tval_el0, x0 \n"); // set timer to 2 sec
}

static inline void timer_el0_enable() {
  asm volatile("mov x0, 1             \n");
  asm volatile("msr cntp_ctl_el0, x0  \n");
}