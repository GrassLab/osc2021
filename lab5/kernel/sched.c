#include "sched.h"
#include <printf.h>
#include <string.h>


void schedule() {
  struct task_struct* current_task, *next_task;
  
  current_task = get_current();
  next_task = task_queue_pop(&run_queue);
  
  task_queue_push(current_task, &run_queue);
  
  if(next_task != null) {
    
    switch_to(current_task, next_task);
  
  }
  // else if
  // exit
  
}
