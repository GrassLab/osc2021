#include "timer.h"

#include <stddef.h>

#include "exc.h"
#include "io.h"
#include "mem.h"

#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int *)0x40000040)

typedef struct timer_pending {
  unsigned long exp_time;
  void *callback;
  void *data;
  struct timer_pending *next;
} timer_pending;

// aleast one timer (infinity timer) when timer enable
timer_pending *tp_list = NULL;
unsigned long timer_frq;

void core_timer_enable() {
  tp_list = kmalloc(sizeof(timer_pending));
  tp_list->callback = NULL;
  tp_list->exp_time = 0xffffffffffffffff;
  tp_list->data = NULL;
  tp_list->next = NULL;

  asm volatile(
      "mov x0, 1;"
      "msr cntp_ctl_el0, x0;"
      "mov x0,  -1;"
      "msr cntp_cval_el0, x0;"
      "mrs %0, cntfrq_el0;"
      : "=r"(timer_frq));

  log_hex("core timer frequency", timer_frq, LOG_DEBUG);

  *CORE0_TIMER_IRQ_CTRL = 2;  // nCNTPNSIRQ IRQ control enable
}

void add_timer(double time, void *callback, void *data) {
  unsigned long exp_time;
  asm volatile("mrs %0, cntpct_el0;" : "=r"(exp_time));
  // exp time using cntp cval instead of tval
  exp_time = exp_time + ((unsigned long)(time * timer_frq));
  timer_pending *new_tp = kmalloc(sizeof(timer_pending));
  new_tp->exp_time = exp_time;
  new_tp->callback = callback;
  new_tp->data = data;
  if (exp_time < tp_list->exp_time) {
    new_tp->next = tp_list;
    tp_list = new_tp;
    asm volatile("msr cntp_cval_el0, %0;" ::"r"(exp_time));
  } else {
    timer_pending *tp_itr = tp_list;
    while (tp_itr->next != NULL && tp_itr->next->exp_time <= exp_time) {
      tp_itr = tp_itr->next;
    }
    new_tp->next = tp_itr->next;
    tp_itr->next = new_tp;
  }
}

void print_time(void *data) {
  add_timer(2, &print_time, NULL);
  unsigned long cnt;
  asm volatile("mrs %0, cntpct_el0;" : "=r"(cnt));
  unsigned long time = cnt / timer_frq;
  log_hex("up time", time, LOG_PRINT);
}

void exec_pending_timer() {
  unsigned long cnt;
  while (1) {
    asm volatile("mrs %0, cntpct_el0;" : "=r"(cnt));
    disable_interrupt();
    if (tp_list->exp_time <= cnt) {
      timer_pending *tp = tp_list;
      tp_list = tp_list->next;
      enable_interrupt();
      ((void (*)(void *))(tp->callback))(tp->data);
      kfree(tp);
    } else {
      asm volatile("msr cntp_cval_el0, %0;" ::"r"((tp_list->exp_time)));
      enable_interrupt();
      break;
    }
  }
  *CORE0_TIMER_IRQ_CTRL = 2;
}

void core_timer_handler() {
  *CORE0_TIMER_IRQ_CTRL = 0;
  add_task(&exec_pending_timer, NULL, 0);
}