#ifndef TIMER_H_
#define TIMER_H_
#include <types.h>
//one-shot timer
struct core_timer_callback {
  void* callback; 
  void* argv;
  size_t argc;
  size_t timeout;
  struct core_timer_callback* next;
};

struct core_timer_callback* _core_timer_queue; 

int core_timer_queue_is_due(struct core_timer_callback *q);
void core_timer_queue_push(void* callbback, size_t timeout);
void* core_timer_queue_pop();
void core_timer_queue_status();
void core_timer_queue_sorted(struct core_timer_callback *q);
void core_timer_queue_update();

void get_core_time();
void core_time_interrupt_handler();

#endif