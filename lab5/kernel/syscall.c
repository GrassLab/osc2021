#include "sched.h"
#include <printf.h>
#include <string.h>
#include <varied.h>
#include <cpio.h>

void sys_exit(int status) {
  do_exit(status);
}

void do_exit(int status) {
  struct task_struct *current_task, *next_task;
  //disable interrupt
  disable_interrupt();
  current_task = get_current();
  
  //set exit status
  current_task->exit_status = status;
  
  //remove from run queue
  task_queue_remove(current_task, &run_queue);
  
  task_queue_status(run_queue.head);
  
  //set task state to dead
  current_task->status = TASK_STATUS_DEAD;
  
  //get next task
  next_task = task_queue_pop(&run_queue);
  
  //enable interrupt
  enable_interrupt();

  //switch to next task
  if(next_task != null) {
    switch_to(current_task, next_task);
  }
}

int sys_fork() {
  return do_fork();
}

int do_fork() {
  //fork a new user task
  struct task_struct* current_task, *new_task; 
  struct trapframe* tf;
  void* start, *stack;
  //disable interrupt
  disable_interrupt();

  current_task = get_current();
  //create task
  new_task = privilege_task_create((void* )not_syscall);
  
  if(new_task != null) {
    //copy memory
    start = fork_memcpy(new_task, current_task->start, current_task->size);
    
    if(start == null) {
      return -1;
    }
    new_task->start = start;
    
    //copy trapframe
    tf = get_trapframe(current_task);
    memcpy((char* )tf, (char* )get_trapframe(current_task), sizeof(struct trapframe));
    //set return value
    tf->x0 = 0;
    
    //allocate new user stack
    stack = varied_malloc(PAGE_SIZE);

    if(stack == null) {
      return -1;
    }

    //set user stack (?)
    new_task->stack = stack;
    tf->sp_el0 = (size_t)stack; 
    
    //copy heap (?)

    //push into run queue
    task_queue_push(new_task, &run_queue);
    //enable interrupt
    enable_interrupt();

    return new_task->task_id;
  }
  else
    //fork fail
    return -1;
}

void* fork_memcpy(struct task_struct *t, void* start, size_t size) {
  void* addr;
  
  //allocate memory for user program
  addr = varied_malloc(size);
  
  if(addr == null)
    return null;
  
  //copy memory from current task 
  memcpy((char *)addr, (char* )start, size);
  
  return addr; 
}

int sys_exec(const char* name, char* const argv[]) {
  return do_exec(name, argv);
}

int do_exec(const char* name, char* const argv[]) {
  void* addr;
  
  //load program from initrootfs
  addr = load_program(name);
  
  if(addr == null)
    return -1;
  
  //set user context

  return 0;
}

void* load_program(const char* name) {
  struct cpio_metadata *metadata;
  void* addr;
  metadata = (struct cpio_metadata* )cpio_get_metadata(name, strlen(name));
  
  addr = varied_malloc(metadata->file_size);
  
  if(addr == null)
    return null;

  memcpy((char *)addr, (char *)metadata->file_address, metadata->file_size);

  return addr; 
}

int sys_getpid() {
  return do_getpid();
}

int do_getpid() {
  return get_current()->task_id;  
}