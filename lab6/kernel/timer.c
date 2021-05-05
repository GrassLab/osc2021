#include "timer.h"
#include <types.h>
#include <uart.h>
#include <printf.h>
#include <dynamic.h>

void show_core_time() {
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
  
  while(1) {
    q = (struct core_timer_callback *)core_timer_queue_pop();
    if(q != null) {
      ((void (*)(char*, size_t))(q->callback))(q->buf, q->size);
      dynamic_free(q);
    }
    else
      break;
  }
  //set next time out      
  //"lsl x0, x0, #1\n" 
  asm volatile("mrs x0, cntfrq_el0\n" "lsr x0, x0, #1\n" "msr cntp_tval_el0, x0\n");
  
  //output now time
  /*odd_flag = ~odd_flag;
  if(odd_flag == 0)
    show_core_time();*/
}

void core_timer_print_message_callback(char *message, size_t size) {
  //print message
  printf("message: %s\n", message);
  //print current time
  show_core_time();
}
 

 size_t sys_get_time() {
   return do_get_time();
 }

 size_t do_get_time() {
  size_t count, freq;
  float time;

  asm volatile("mrs  %[result], cntpct_el0": [result]"=r"(count));
  asm volatile("mrs  %[result], cntfrq_el0": [result]"=r"(freq));
  time = (1000 * count) / freq;
   
   return time;
 }

void delay(size_t sec) {
  size_t start_t, now_t;
  start_t = do_get_time();
  now_t = start_t;
  
  while((now_t - start_t) < 1000 * sec) {
    now_t = do_get_time();
  }
}