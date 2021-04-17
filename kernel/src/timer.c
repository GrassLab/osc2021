#include "timer.h"

#include "alloc.h"
#include "io.h"
#include "utils.h"

void timeout_event_init() {
  timeout_queue_head = 0;
  timeout_queue_tail = 0;
}

void core_timer_enable() {
  asm volatile("mov x0, 1");
  asm volatile("msr cntp_ctl_el0, x0");  // enable
  asm volatile("mrs x0, cntfrq_el0");
  asm volatile("msr cntp_tval_el0, x0");  // set expired time
  asm volatile("mov x0, 2");
  asm volatile("ldr x1, =0x40000040");  // CORE0_TIMER_IRQ_CTRL
  asm volatile("str w0, [x1]");         // unmask timer interrupt
}

void core_timer_disable() {
  asm volatile("mov x0, 0");
  asm volatile("msr cntp_ctl_el0, x0");  // disable
  asm volatile("mov x0, 0");
  asm volatile("ldr x1, =0x40000040");  // CORE0_TIMER_IRQ_CTRL
  asm volatile("str w0, [x1]");         // unmask timer interrupt
}

void core_timer_handler_lowerEL_64() {  // required 2
  print_s("Time elapsed after booting: ");
  print_i(get_current_time());
  print_s("s\n");
  set_expired_time(2);  // set the next timeout to 2 seconds later
}

void core_timer_handler_currentEL_ELx() {  // elective 2
  print_s("Current time: ");
  print_i(get_current_time());
  print_s("s, ");
  print_s("Command executed time: ");
  print_i(timeout_queue_head->register_time);
  print_s("s, ");
  print_s("Duration: ");
  print_i(timeout_queue_head->duration);
  print_s("s\n");

  timeout_queue_head->callback(timeout_queue_head->args);
  timeout_event *next = timeout_queue_head->next;
  free(timeout_queue_head);

  if (next) {  // if there are other timeout events, set the next timeout
    next->prev = 0;
    timeout_queue_head = next;
    uint64_t next_duration =
        next->register_time + next->duration - get_current_time();
    set_expired_time(next_duration);
  } else {  // if there are no other timeout events, disable core timer
    timeout_queue_head = 0;
    timeout_queue_tail = 0;
    core_timer_disable();
  }
}

void add_timer(void (*callback)(char *), char *args, uint32_t duration) {
  // create a new timeout event
  timeout_event *new_timeout_event =
      (timeout_event *)malloc(sizeof(timeout_event));
  new_timeout_event->register_time = get_current_time();
  new_timeout_event->duration = duration;
  new_timeout_event->callback = callback;
  for (int i = 0; i < 20; i++) {
    new_timeout_event->args[i] = args[i];
    if (args[i] == '\0') break;
  }
  new_timeout_event->prev = 0;
  new_timeout_event->next = 0;

  if (timeout_queue_head == 0) {  // there are no other timeout events
    timeout_queue_head = new_timeout_event;
    timeout_queue_tail = new_timeout_event;
    core_timer_enable();
    set_expired_time(duration);
  } else {
    // find the proper insert position of the timeout event queue
    timeout_event *cur;
    uint64_t timeout =
        new_timeout_event->register_time + new_timeout_event->duration;
    for (cur = timeout_queue_head; cur; cur = cur->next) {
      if (cur->register_time + cur->duration > timeout) break;
    }

    if (cur == 0) {  // insert to the queue tail
      new_timeout_event->prev = timeout_queue_tail;
      timeout_queue_tail->next = new_timeout_event;
      timeout_queue_tail = new_timeout_event;
    } else if (cur->prev == 0) {  // insert to the queue head
      new_timeout_event->next = cur;
      timeout_queue_head->prev = new_timeout_event;
      timeout_queue_head = new_timeout_event;
      set_expired_time(duration);
    } else {
      new_timeout_event->prev = cur->prev;
      new_timeout_event->next = cur;
      cur->prev->next = new_timeout_event;
      cur->prev = new_timeout_event;
    }
  }
}

uint64_t get_current_time() {  // get current time(sec) after booting
  uint64_t cntpct_el0, cntfrq_el0;
  asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct_el0));
  asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
  return cntpct_el0 / cntfrq_el0;
}

void set_expired_time(uint32_t duration) {  // set expired time to duration(sec)
  uint64_t cntfrq_el0;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
  asm volatile("msr cntp_tval_el0, %0" : : "r"(cntfrq_el0 * duration));
}

void timer_callback(char *msg) {
  print_s("Message: ");
  print_s(msg);
  print_s("\n");
}
