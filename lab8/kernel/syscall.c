#include "sched.h"
#include <printf.h>
#include <string.h>
#include <varied.h>
#include <cpio.h>
#include <elf.h>
#include <uart.h>
#include <vfs.h>
#include <page.h>

void sys_exit(int status) {
  do_exit(status);
}

void do_exit(int status) {
  struct task_struct *current_task, *next_task;
  current_task = get_current();
  
   //set exit status
  current_task->exit_status = status;
  //set task state to dead  
  current_task->status = TASK_STATUS_DEAD;

  disable_interrupt();
  
  //remove from run queue
  task_queue_remove(current_task, &run_queue);
  //should free user space memory
  //varied_free(current_task->start);
  //get next task
  next_task = task_queue_pop(&run_queue);
  
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
  
 
  disable_interrupt();
  //create task
  new_task = privilege_task_create(get_current()->start);

  enable_interrupt();

  if(new_task != null) {

    //allocate new page to copy memory
    start = fork_memcpy(new_task, get_current()->start, get_current()->size);
    
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
    new_task->ctx.lr = (size_t)kernel_exit;
    //new process will start at kernel_exit, kernel stack should be trapframe
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
  disable_interrupt();
  
  addr = varied_malloc(allocated_size);
  
  enable_interrupt();
  
  addr += PAGE_SIZE - (size_t)addr % PAGE_SIZE;

  if(addr == null)
    return null;
  
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
  void* addr, *start, *stack;
  char *arg;
  int argc;
  struct trapframe* current_tf;
  printf("do_exec\n");
  
  //load program from initrootfs
  addr = load_program(name);
  
  if(addr == null)
    return -1;
  
  //parse elf header
  start = elf_header_parse(addr);
  printf("start_address: 0x%x\n", start);
  
  stack = get_current()->stack + TASK_STACK_SIZE;
  arg = argv[0];
  argc = 0;
  
  while(arg != null) 
    arg = argv[++argc];
  
  //set pass argument
  stack = exec_set_argv(stack, argc, argv);
  
  printf("stack: 0x%x, 0x%x\n", stack, get_current()->stack);
  
  page_map_stack(get_current()->stack + TASK_STACK_SIZE);

  /** set user context
   * set kernel stack sp
   * set user stack sp_el0
   * return address to exit
   * set argument
   */
  /*asm volatile("mov x0, %0\n" "msr sp_el0, x0\n"
               "mov sp, %1\n" 
               "mov x0, %2\n" "msr elr_el1, x0\n" 
               "mov x0, %3\n"
               "mov x1, %4\n"
               "dsb ish \n" 
               "mov x0, %5\n" "msr ttbr0_el1, x0\n"
               "tlbi vmalle1is\n" 
               "dsb ish\n" 
               "isb\n" 
               "eret\n"
               ::"r"(stack),
               "r"(get_current()->kstack + TASK_STACK_SIZE),
               "r"((0x10031000)),
               "r"(argc),
               "r"(stack + 0x10),
               "r"((size_t)ttbr0 & 0x0000fffffffffff0)
                :"x0");*/

 
  current_tf = get_trapframe(get_current());
  
  printf("current_tf: 0x%x\n", current_tf);
  current_tf->x0 = argc;
  current_tf->sp_el0 = 0x00007ffffffff000 - ((size_t)(get_current()->stack + TASK_STACK_SIZE - stack) & 0x0000ffffffffffff);
  current_tf->x1 = current_tf->sp_el0 + 0x10;
  current_tf->elr_el1 = 0x400000 + ((size_t)(start - addr) & 0x0000fffffffff000);
  disable_interrupt();
  
  asm volatile("mov x0, %0\n" "mov sp, %1\n" "blr %2\n"::
  "r"(get_current()->ctx.pgd), 
  "r"(current_tf),
  "r"((void* )exec_exit): "x0");
  
  //asm volatile("mov sp, %0\n" "blr %1\n"::"r"(current_tf), "r"((void* )kernel_exit));
  
  enable_interrupt();
  return 0;
}

void* exec_set_argv(void* stack, int argc, char* const argv[]) {
  char **argv_addr;
  int count, r;
  count = argc;
  
  //allocate need to be larger than 0x20
  if(count < 4) 
    count = 4;
  
  disable_interrupt();

  argv_addr = (char** )varied_malloc(count * sizeof(char *));

  enable_interrupt();

  //set argv[i] content
  for(int i = argc - 1; i >= 0; i--) {
    r = (strlen(argv[i])+ 1) % 16;
    //padding
    if(r != 0) 
      stack -= 16 - r;
      
    stack -= strlen(argv[i]) + 1;
    //record argv[i] address
    argv_addr[i] = stack;

    memcpy((char *)stack, argv[i], strlen(argv[i]) + 1);
    memset((char *)stack + (strlen(argv[i])+ 1), 16 - r, 0);
  }

  if(argc % 2 == 0)
    stack -= sizeof(size_t);
  //set null
  stack -= sizeof(char *);
  *(char **)stack = null;

  //set argv[i] address
  for(int i = argc - 1; i >= 0; i--) {
    stack -= sizeof(char *);
    *(char **)stack = argv_addr[i]; 
  }

  /*stack -= sizeof(char **);
  *(char** )stack = (char*)stack + sizeof(char **);*/
  //add padding
  //set argc
  stack -= 16 - sizeof(int) % 16;
  stack -= sizeof(int);
  *(int *)stack = argc;
  
  return stack;
}

void* load_program(const char* name) {
  struct cpio_metadata *metadata;
  void* addr;
  size_t size;

  //find elf in cpio
  metadata = (struct cpio_metadata* )cpio_get_metadata(name, strlen(name));
  
   if(metadata == null)
    return null;
  
  size = metadata->file_size;
  
  if((size_t)get_current()->start >= BUDDY_START) {
    if(get_current()->size >= metadata->file_size) {
      //printf("already is user process\n");
      addr = get_current()->start;
      
      memcpy((char *)addr, (char *)metadata->file_address, metadata->file_size);
      
      get_current()->size = metadata->file_size;
      
      return addr;
    }
    else {

      disable_interrupt();
      //size not enough, need to reallocate
      varied_free(get_current()->start);

      enable_interrupt();
    }
  }

  disable_interrupt();

  addr = buddy_malloc(size);

  enable_interrupt();
  
  if(addr == null)
    return null;
  
  printf("addr: 0x%x\n", addr);

  memcpy((char *)addr, (char *)metadata->file_address, metadata->file_size);
  //store user space info
  get_current()->start = addr;
  get_current()->size = metadata->file_size;
  
  disable_interrupt();
  get_current()->ctx.pgd = (size_t)page_map_binary(addr, size) & 0x0000fffffffff000;
  enable_interrupt();
  return addr; 
}

int sys_getpid() {
  return do_getpid(); 
}

int do_getpid() {
  return get_current()->task_id;  
}


int sys_open(const char *pathname, int flags) { 
  return do_open(pathname, flags);
}

int do_open(const char *pathname, int flags) {
  struct file* file;
  
  if(pathname == null)
    return -1;
  if(strlen(pathname) == 0)
    return -1;

  for(int i = 0; i < FD_TABLE_SIZE; i++) {
    if(get_current()->fd_table[i] == null) {
      //should preserve 0, 1, 2 to stdin, stdout, stderr 
      file = vfs_open(pathname, flags);
      //file cannot open
      if(file == null)
        return -1;

      get_current()->fd_table[i] = file;

      return i;
    }
  }
  //fd table full
  return -1;
}

int sys_close(int fd) {
  return do_close(fd);
}

int do_close(int fd) {

  if(fd < 0 || fd > FD_TABLE_SIZE)
    return -1;
  //not open
  if(get_current()->fd_table[fd] == null)
    return -1;

  return vfs_close(get_current()->fd_table[fd]); 
}

int sys_write(int fd, const void *buf, int count) {
  return do_write(fd, buf, count);
}

int do_write(int fd, const void *buf, int count) {
  
  if(fd < 0 || fd > FD_TABLE_SIZE)
    return -1;
  
  //not open
  if(get_current()->fd_table[fd] == null)
    return -1;

  if(buf == null)
    return -1;
  if(count < 0)
    return -1;
  if(count == 0)
    return 0;
  
  return vfs_write(get_current()->fd_table[fd], buf, count);
}

int sys_read(int fd, void *buf, int count) {
  return do_read(fd, buf, count);
}

int do_read(int fd, void *buf, int count) {
  
  if(fd < 0 || fd > FD_TABLE_SIZE)
    return -1;
  
  //not open
  if(get_current()->fd_table[fd] == null)
    return -1;

  if(buf == null)
    return -1;
  if(count < 0)
    return -1;
  else if(count == 0)
    return 0;
  
  return vfs_read(get_current()->fd_table[fd], buf, count);
  
}