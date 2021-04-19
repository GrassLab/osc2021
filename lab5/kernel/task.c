#include "sched.h"
#include <printf.h>

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
  task_pool[i].kstack = (void* )KERNEL_STACK_ADDR + TASK_STACK_SIZE * (i + 1);
  task_pool[i].ctx.sp = (size_t)task_pool[i].kstack; 
  task_pool[i].ctx.lr = (size_t)func;
  //task_pool[i].stack = (void *)USER_STACK_ADDR + TASK_STACK_SIZE * (i + 1);
  task_pool[i].next = null;
  //push into run queue
  task_queue_push(&task_pool[i], &run_queue);
 
  return &task_pool[i];
}


void idle_task() {
  while(1) {
    for(int i = 0; i < TASK_POOL_SIZE; i++) {
    //kill zombie task
      if(task_pool[i].status == TASK_STATUS_DEAD &&  task_pool[i].task_id != 0) {
        //zombie process
        printf("kill task: %d\n", task_pool[i].task_id);
        task_pool[i].task_id = 0;
      }
    }

    schedule();  
  }
}

void test_task() {
  //while(1) {
  printf("test task id: %d\n", get_current()->task_id);
  do_exit(0);
    //schedule();
  //}
}


void task_init() {
  struct task_struct fake;
  //create idle task
  privilege_task_create(idle_task);

  for(int i = 0; i < 5; i++) {
    privilege_task_create(test_task);
  }
  
  task_queue_status(run_queue.head);

  switch_to(&fake, task_queue_pop(&run_queue));

}

struct trapframe* get_trapframe(struct task_struct* t) {
  struct trapframe* tf;
  tf = (struct trapframe* )(t->kstack - sizeof(struct trapframe));
  return tf;
}


