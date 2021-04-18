#include "sched.h"
#include <printf.h>
void sys_exit(int status) {
  do_exit(status);
}

void do_exit(int status) {
  struct task_struct *current_task, *next_task;
  //disable interrupt
  disable_interrupt();
  current_task = get_current();
  //printf("do_exit: %d\n", current_task->task_id);
  //set exit status
  current_task->exit_status = status;
  //remove from run queue
  task_queue_remove(current_task, &run_queue);
  task_queue_status(run_queue.head);
  //set task state to dead
  current_task->status = TASK_STATUS_DEAD;
  //switch to next task
  next_task = task_queue_pop(&run_queue);
  if(next_task != null) {
    switch_to(current_task, next_task);
  }
  //enable interrupt
  enable_interrupt();
}

int sys_fork() {
  return do_fork();
}

int do_fork() {
  //fork a new user task
  //disable interrupt
  disable_interrupt();

  privilege_task_create((void* )get_current()->ctx.lr);
  
  //enable interrupt
  enable_interrupt();

  return 0;
}

int sys_exec(void(*func)()) {
  return do_exec(func);
}

int do_exec(void(*func)()) {
  //context switch
  return 0;
}