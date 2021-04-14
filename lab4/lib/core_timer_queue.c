#include <timer.h>
#include <dynamic.h>
#include <printf.h>
#include <string.h>

int core_timer_queue_is_due(struct core_timer_callback *q) {
  if(q != null)
    return q->timeout == 0;
  else
    return 0;
}

void core_timer_queue_push(void* callback, size_t timeout, char *message, size_t size) {
  struct core_timer_callback *q = (struct core_timer_callback *)dynamic_malloc(sizeof(struct core_timer_callback));
  q->callback = callback;
  q->timeout = timeout; 
  q->argc = 0;
  q->argv = null;
  q->size = size;
  strncpy(q->buf, message, size);
  q->next = _core_timer_queue;
   _core_timer_queue = q;
  //printf("push %d\n", q->timeout);
  core_timer_queue_status();
  core_timer_queue_sorted(_core_timer_queue);
  
}
void* core_timer_queue_pop() {
  struct core_timer_callback *q;
  if(_core_timer_queue != null && core_timer_queue_is_due(_core_timer_queue)) {
    q = _core_timer_queue;
    if(_core_timer_queue->next != null)
      _core_timer_queue = _core_timer_queue->next;
    else
      _core_timer_queue = null;
    return q;
  }
  else  
    return null;
}

void core_timer_queue_status() {
  struct core_timer_callback *q = _core_timer_queue;
  if(q != null) {
    while(q != null) {
      printf("timeout: %d -> ", q->timeout);   
      q = q->next;
    }
    printf("null\n");
  }
}

void core_timer_queue_sorted(struct core_timer_callback *q) {
  struct core_timer_callback *ptr, *prev_ptr, *next_ptr;
  ptr = q;
  prev_ptr = null;
  
  while(ptr != null) {
    if(ptr->next != null) {
      next_ptr = ptr->next;
      if(ptr->timeout > next_ptr->timeout) {
        ptr->next = next_ptr->next;
        next_ptr->next = ptr;
        if(prev_ptr != null) 
          prev_ptr->next = next_ptr;
        else 
          _core_timer_queue = next_ptr;

        ptr = next_ptr;
      }
    }
    prev_ptr = ptr;
    ptr = ptr->next;    
  }
}

void core_timer_queue_update() {
  struct core_timer_callback *q = _core_timer_queue;
  while(q != null) {
    q->timeout--;
    q = q->next;
  }
}