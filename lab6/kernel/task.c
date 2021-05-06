#include "sched.h"
#include <printf.h>
#include <string.h>
#include <varied.h>
#include <sched_test.h>

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
  task_pool[i].status = TASK_STATUS_READY;
  task_pool[i].kstack = (void* )KERNEL_STACK_ADDR + TASK_STACK_SIZE * i;
  task_pool[i].ctx.sp = (size_t)task_pool[i].kstack + TASK_STACK_SIZE; 
  task_pool[i].ctx.lr = (size_t)func;
  task_pool[i].stack = (void *)USER_STACK_ADDR + TASK_STACK_SIZE * i;
  task_pool[i].next = null;
  task_pool[i].start = null;
  task_pool[i].resched = 0;
  
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
        //free user space memory
        if(task_pool[i].start != null)
          varied_free(task_pool[i].start);
        task_pool[i].start = 0;
        task_pool[i].kstack = 0;
        task_pool[i].stack = 0;
        task_pool[i].task_id = 0;
      }
    }
    schedule();  
  }
}

void user_task() {

  char* argv[] = {"argv_test", "-o", "arg2", 0};
  do_exec("argv_test", argv);
  
}


void task_init() {
  struct task_struct fake;
  
  disable_interrupt();
  //create idle task
  privilege_task_create(idle_task);

  for(int i = 0; i < 5; i++) {
    privilege_task_create(foo);
  }
  
  privilege_task_create(user_task);
    
  task_queue_pop(&run_queue);
  
  task_queue_status(&run_queue);

  enable_interrupt();

  switch_to(&fake, &task_pool[0]);

}

void task_test1_init() {
  struct task_struct fake;
  
  disable_interrupt();
  //create idle task
  privilege_task_create(idle_task);

  for(int i = 0; i < 5; i++) {
    privilege_task_create(foo);
  }
    
  task_queue_pop(&run_queue);
  
  task_queue_status(&run_queue);

  enable_interrupt();

  switch_to(&fake, &task_pool[0]);


}


void task_test2_init() {
  struct task_struct fake;
  
  disable_interrupt();
  //create idle task
  privilege_task_create(idle_task);

  privilege_task_create(user_task);
    
  task_queue_pop(&run_queue);
  
  task_queue_status(&run_queue);

  enable_interrupt();

  switch_to(&fake, &task_pool[0]);


}

void task_vfs_test_init() {
  struct task_struct fake;
  
  disable_interrupt();
  //create idle task
  privilege_task_create(idle_task);

  privilege_task_create(user_vfs_task);
    
  task_queue_pop(&run_queue);
  
  task_queue_status(&run_queue);

  enable_interrupt();

  switch_to(&fake, &task_pool[0]);


}

void user_vfs_task() {
  
  char *vfs_argv[] = {"vfs_test", 0};
  do_exec("vfs_test", vfs_argv);

}

struct trapframe* get_trapframe(struct task_struct* t) {
  struct trapframe* tf;
  tf = (struct trapframe* )(t->kstack + TASK_STACK_SIZE - sizeof(struct trapframe));
  return tf;
}



