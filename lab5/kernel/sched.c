#include "sched.h"
#include <printf.h>
#include <string.h>

struct task_struct* privilege_task_create( void(*func)() ) {
  size_t i;
  //find empty element in task pool
  for(i = 0; i < TASK_POOL_SIZE; i++) {
    if(task_pool[i].task_id == 0)
      break;
  }
  
  if(i == TASK_POOL_SIZE)
    return null;
  
  //set task element
  task_pool[i].task_id = i + 1;
  task_pool[i].status = TASK_STATUS_LIVE;
  task_pool[i].ctx.sp = (size_t)USER_STACK_ADDR + TASK_STACK_SIZE * i; 
  task_pool[i].ctx.lr = (size_t)func;
  task_pool[i].kstack = (void* )KERNEL_STACK_ADDR + TASK_STACK_SIZE * i;
  
  //push into run queue
  run_queue_push(&task_pool[i], &run_queue);
   
  return &task_pool[i];
}

void schedule() {
  struct task_struct* current_task, *next_task;
  
  current_task = get_current();
  next_task = run_queue_pop(&run_queue);
  
  run_queue_push(current_task, &run_queue);
  
  if(next_task != null) {
    
    switch_to(current_task, next_task);
  
  }
  // else if
  // exit
  
}

void idle_task() {
  while(1) {
    for(int i = 0; i < TASK_POOL_SIZE; i++) {
    //kill zombie task
      if(task_pool[i].status == TASK_STATUS_DEAD) {
        //zombie process
        task_pool[i].task_id = 0;
      }
    }
    printf("idle task\n");
    schedule();  
  }
}

void test_task() {
  while(1) {
    printf("task id: %d\n", get_current()->task_id);
    schedule();
  }
}


void task_init() {
  struct task_struct fake;
  //create idle task
  privilege_task_create(idle_task);

  for(int i = 0; i < 63; i++) {
    privilege_task_create(test_task);
  }
  
  switch_to(&fake, &task_pool[0]);
  //schedule();
}