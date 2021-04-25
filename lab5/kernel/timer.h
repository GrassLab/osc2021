#ifndef TIMER_H_
#define TIMER_H_
#include <types.h>
//one-shot timer

#define CORE_TIMER_CALLBACK_BUFFER_SIZE 0x100
struct core_timer_callback {
  void* callback; 
  void* argv;
  size_t argc;
  size_t timeout;
  char buf[CORE_TIMER_CALLBACK_BUFFER_SIZE];
  size_t size;
  struct core_timer_callback* next;
};

struct core_timer_callback* _core_timer_queue; 
int odd_flag;

int core_timer_queue_is_due(struct core_timer_callback *q);
void core_timer_queue_push(void* callback, size_t timeout, char *message, size_t size);
void* core_timer_queue_pop();
void core_timer_queue_status();
void core_timer_queue_sorted(struct core_timer_callback *q);
void core_timer_queue_update();

void show_core_time();
void core_time_interrupt_handler();
void core_timer_print_message_callback(char *message, size_t size);

size_t sys_get_time();
size_t do_get_time();
extern void disable_interrupt();
extern void enable_interrupt();
void delay(size_t sec);
#endif