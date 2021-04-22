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
  
  //should free user space memory

  //task_queue_status(run_queue.head);
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
  struct task_struct *new_task; 
  struct trapframe* current_tf, *new_tf;
  void* start;
  
  //disable interrupt
  disable_interrupt();
  //create task
  new_task = privilege_task_create((void* )not_syscall);
  //enable interrupt
  enable_interrupt();

  if(new_task != null) {

    //disable interrupt
    disable_interrupt();     
    //allocate new page to copy memory
    start = fork_memcpy(new_task, get_current()->start, get_current()->size);
    //enable interrupt
    enable_interrupt();

    if(start == null) {
      return -1;
    }

    //copy trapframe in kernel stack
    current_tf = get_trapframe(get_current());
    new_tf = get_trapframe(new_task);
    memcpy((char* )new_tf, (char* )current_tf, sizeof(struct trapframe));
    //set return value, elr_el1, sp_el0
    new_tf->x0 = 0;
    new_tf->elr_el1 = (size_t)new_task->start + (current_tf->elr_el1 - (size_t)get_current()->start);
    new_tf->sp_el0 = ((size_t)new_task->stack + TASK_STACK_SIZE) + (current_tf->sp_el0 - ((size_t)get_current()->stack + TASK_STACK_SIZE));  
    //copy user stack memory
    memcpy((char *)new_tf->sp_el0, (char* )current_tf->sp_el0, (size_t)get_current()->stack + TASK_STACK_SIZE - current_tf->sp_el0);
    //copy heap (?)
    
    //copy context
    memcpy((char* )&new_task->ctx, (char *)&get_current()->ctx, sizeof(struct context));
    //set lr, sp
    new_task->ctx.lr = (size_t)not_syscall;
    //new process will start at not_syscall kernel stack should be trapframe
    new_task->ctx.sp = (size_t)new_tf;
    
    return new_task->task_id;
  }
  else
    //fork fail
    return -1;
}

void* fork_memcpy(struct task_struct *t, void* start, size_t size) {
  void* addr;
  int allocated_size;

  allocated_size = size + PAGE_SIZE;
  //allocate memory for new user program
  addr = varied_malloc(allocated_size);
  addr += PAGE_SIZE - (size_t)addr % PAGE_SIZE;

  if(addr == null)
    return null;
  
  printf("fork new memory address: 0x%x\n", addr);
  //copy memory from current task  
  memcpy((char *)addr, (char* )start, size);
  
  //update task user info
  t->start = addr;
  t->size = size;

  return addr;
}

int sys_exec(const char* name, char* const argv[]) {
  return do_exec(name, argv);
}

int do_exec(const char* name, char* const argv[]) {
  void* addr;
  struct trapframe *tf;
  printf("do_exec\n");
  
  //load program from initrootfs
  addr = load_program(name);
  
  if(addr == null)
    return -1;
  
  //set pass argument

  //reset context
  memset((char* )&get_current()->ctx, sizeof(struct context), 0);
  //set user context
  //set kernel stack sp
  //set user stack sp_el0
  printf("stack: 0x%x\n", get_current()->stack);
  printf("kstack: 0x%x\n", get_current()->kstack);
  
  //set trapframe in kernel stack
  //set elr_el1 to binary entry point
  //return address to exit
  asm volatile("mov x0, %0\n" "msr sp_el0, x0\n"
               "mov sp, %1\n" 
               "mov x0, %2\n" "msr elr_el1, x0\n" 
               "mov x30, %3\n"
               "eret\n"
               ::"r"(get_current()->stack + TASK_STACK_SIZE),
               "r"(get_current()->kstack + TASK_STACK_SIZE),
               "r"(addr),
               "r"((void* )exit)
                :"x0");
  return 0;
}

void* load_program(const char* name) {
  struct cpio_metadata *metadata;
  void* addr;
  size_t size;
  metadata = (struct cpio_metadata* )cpio_get_metadata(name, strlen(name));
  
  size = metadata->file_size + PAGE_SIZE;
  //printf("file addr: 0x%x\n", metadata->file_address);
  if((size_t)get_current()->start >= BUDDY_START) {
    if(get_current()->size >= metadata->file_size) {
      printf("already is user process\n");
      addr = get_current()->start;
      
      memcpy((char *)addr, (char *)metadata->file_address, metadata->file_size);
      
      get_current()->size = metadata->file_size;
      
      return addr;
    }
    else {
      //size not enough, need to reallocate
      varied_free(get_current()->start);
    }
  }
  /** use a stupid method to prevent wrong offset, allocate more space in order to
   * make base address become 0xX000, since binary used here is smaller than 0x1000, the alignment 
   * only need to be done to 0x1000. 
   */
  
  addr = varied_malloc(size);
  addr += PAGE_SIZE - (size_t)addr % PAGE_SIZE;

  if(addr == null)
    return null;

  printf("addr: 0x%x\n", addr);

  memcpy((char *)addr, (char *)metadata->file_address, metadata->file_size);

  //store user space info
  get_current()->start = addr;
  get_current()->size = metadata->file_size;

  return addr; 
}

int sys_getpid() {
  return do_getpid(); 
}

int do_getpid() {
  return get_current()->task_id;  
}

size_t sys_uart_read(char buf[], size_t size) {
  return do_uart_read(buf, size);
}
size_t do_uart_read(char buf[], size_t size) {
  return 0;
}
size_t sys_uart_write(const char buf[], size_t size) {
  return do_uart_write(buf, size);
}
size_t do_uart_write(const char buf[], size_t size) {
  return 0;
}