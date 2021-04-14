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
  //core_timer_queue_status();
  while(1) {
    q = (struct core_timer_callback *)core_timer_queue_pop();
    if(q != null) {
      printf("core timer interrupt: %d\n", q->timeout);
      ((void (*)(char*, size_t))(q->callback))(q->buf, q->size);
      dynamic_free(q);
    }
    else
      break;
  }
  //set next time out      
  //"lsl x0, x0, #1\n" 
  asm volatile("mrs x0, cntfrq_el0\n" "msr cntp_tval_el0, x0\n");
  //output now time
  odd_flag = ~odd_flag;
  /*if(odd_flag == 0)
    get_core_time();*/
}

void core_timer_print_message_callback(char *message, size_t size) {
  //print message
  printf("message: %s\n", message);
  //print current time
  get_core_time();
}
 