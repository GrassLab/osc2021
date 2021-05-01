#include "timer.h"


void timeout_event_init() {
  timeout_queue_head = 0;
  timeout_queue_tail = 0;
}
void set_timeout(char *args) {
  int second = 0, last_idx = 0;
  for(int i = 0; args[i] != '\0'; ++i){
	  if(args[i] >= '0' && args[i] <= '9') second = second * 10 + args[i] - '0';
	  else second = 0;
	  if(args[i] == ' ') last_idx = i;
  }
  args[last_idx] = '\0';
  add_timer(timer_callback, args, second);
}

void timer_callback(char *msg) {
	uart_puts(USER_NAME);
	uart_puts("@");
	uart_puts(MACHINE_NAME);
	uart_puts("$ ");
	uart_puts("timer_callback\n");
	uart_puts(USER_NAME);
	uart_puts("@");
	uart_puts(MACHINE_NAME);
	uart_puts("$ ");

}

void add_timer(void (*callback)(char *), char *args, unsigned int duration) {
  timeout_event *new_timeout_event =(timeout_event *)kmalloc(sizeof(timeout_event));
  new_timeout_event->register_time = get_current_time();
  new_timeout_event->duration = duration;
  new_timeout_event->callback = callback;
  for (int i = 0; i < 20; i++) {
    new_timeout_event->args[i] = args[i];
    if (args[i] == '\0') break;
  }
  new_timeout_event->prev = 0;
  new_timeout_event->next = 0;

  if (timeout_queue_head == 0) {
    timeout_queue_head = new_timeout_event;
    timeout_queue_tail = new_timeout_event;
    core_timer_enable();
    set_expired_time(duration);
  } 
  else {
    timeout_event *cur;
    unsigned long timeout = new_timeout_event->register_time + new_timeout_event->duration;
    for (cur = timeout_queue_head; cur; cur = cur->next) {
      if (cur->register_time + cur->duration > timeout) break;
    }

    if (cur == 0) {
      new_timeout_event->prev = timeout_queue_tail;
      timeout_queue_tail->next = new_timeout_event;
      timeout_queue_tail = new_timeout_event;
    } 
	else if (cur->prev == 0) {
      new_timeout_event->next = cur;
      timeout_queue_head->prev = new_timeout_event;
      timeout_queue_head = new_timeout_event;
      set_expired_time(duration);
    } 
	else {
      new_timeout_event->prev = cur->prev;
      new_timeout_event->next = cur;
      cur->prev->next = new_timeout_event;
      cur->prev = new_timeout_event;
    }
  }
}

void el1_timer_irq(){
  	uart_puts("Current time: ");
  	uart_put_int(get_current_time());
  	uart_puts("s, ");
  	uart_puts("Command executed time: ");
  	uart_put_int(timeout_queue_head->register_time);
  	uart_puts("s, ");
  	uart_puts("Duration: ");
  	uart_put_int(timeout_queue_head->duration);
  	uart_puts("s\n");
    timeout_queue_head->callback(timeout_queue_head->args);
  	timeout_event *next = timeout_queue_head->next;
  	kfree(timeout_queue_head);
  	if (next) {
    	next->prev = 0;
    	timeout_queue_head = next;
    	unsigned long next_duration = next->register_time + next->duration - get_current_time();
    	set_expired_time(next_duration);
  	} 
	else {
    	timeout_queue_head = 0;
    	timeout_queue_tail = 0;
    	core_timer_disable();
  	}
}

void el0_timer_irq(){
	set_expired_time(2);
	uart_puts("Time elapsed after booting: ");
	uart_put_int(get_current_time());	
	uart_puts("s\n");
}
unsigned long get_current_time() {
  unsigned long cntpct_el0, cntfrq_el0;
  asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct_el0));
  asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
  return cntpct_el0 / cntfrq_el0;
}
void set_expired_time(unsigned int duration) {
  unsigned long cntfrq_el0;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
  asm volatile("msr cntp_tval_el0, %0" : : "r"(cntfrq_el0 * duration));
}
