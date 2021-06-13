#include "sched.h"
#include <printf.h>
#include <string.h>

void schedule() {
  struct task_struct* current_task, *next_task;
  
  current_task = get_current();
  if(current_task->resched == 1) {
    
    disable_interrupt();
    
    //task_queue_status(&run_queue);

    next_task = task_queue_pop(&run_queue);

    task_queue_push(current_task, &run_queue);
    
    current_task->resched = 0;
    
    enable_interrupt();

    if(next_task != null) {
      switch_to(current_task, next_task);
    }
  }
  // else if
  // exit
  
}
