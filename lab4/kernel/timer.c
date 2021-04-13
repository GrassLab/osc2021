#include "timer.h"
#include <types.h>
#include <uart.h>
#include <printf.h>
#include <dynamic.h>

void get_core_time() {
  size_t count, freq;
  float time;
  asm volatile("mrs  %[result], cntpct_el0": [result]"=r"(count));
  asm volatile("mrs  %[result], cntfrq_el0": [result]"=r"(freq));
  time = (1000 * count) / freq;
  printf("[%f] core timer interrupt\n", time / 1000);
}

void core_time_interrupt_handler() {
  struct core_timer_callback *q = null;
  core_timer_queue_update();
  core_timer_queue_status();
  while(1) {
    q = core_timer_queue_pop();
    if(q != null) {
      printf("core timer interrupt: %d\n", q->timeout);
      dynamic_free(q);
    }
    else
      break;
  }
  //set next time out
  asm volatile("mrs x0, cntfrq_el0\n" "lsl x0, x0, #1\n" "msr cntp_tval_el0, x0\n");
  //output now time
  get_core_time();
}

 