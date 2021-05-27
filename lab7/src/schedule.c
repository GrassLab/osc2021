# include "schedule.h"
# include "exception.h"
# include "shell.h"
# include "uart.h"
# include "my_math.h"
# include "my_string.h"
# include "start.h"
# include "vfs.h"
# include "mem.h"
# include "log.h"
# include "linklist.c"

struct task task_pool[TASK_MAX_NUM];
struct task *task_queue_head[PRIORITY_MAX+1];
struct task *task_unuse;

char kstack_pool[TASK_MAX_NUM][STACK_SIZE];
char ustack_pool[TASK_MAX_NUM][STACK_SIZE];

void idle(){
  while(1){
    schedule();
  }
}

void task_init(){
  log_puts((char *) "[INFO] Task Init.\n", INFO);
  for (int i=0; i<TASK_MAX_NUM; i++){
    task_pool[i].pid = i;
    task_pool[i].state = EXIT;
    task_pool[i].resched_flag = 0;
    task_pool[i].pre = (i == 0) ? 0 : &task_pool[i-1];
    task_pool[i].next = (i == TASK_MAX_NUM-1) ? 0 : &task_pool[i+1];
  }
  task_unuse = &task_pool[0];
  int pid = privilege_task_create(idle, PRIORITY_MAX);
  register unsigned long long r = (unsigned long long)(&task_pool[pid]);
  asm volatile("msr tpidr_el1, %0" : : "r"(r));
  privilege_task_create(shell, PRIORITY_MAX-1);
  log_puts((char *) "[INFO] Task Init DONE.\n", INFO);
}

void task_exit(){
  struct task *current = get_current();
  current->state = ZOMBIE;
  schedule();
}

void user_task_start(){
  struct task *current = get_current();
  asm volatile("msr sp_el0, %0" : : "r"(&ustack_pool[current->pid][STACK_TOP_IDX]));
  asm volatile("msr elr_el1, %0": : "r"(current->invoke_func));
  asm volatile("msr spsr_el1, %0" : : "r"(SPSR_EL1_VALUE));
  asm volatile("eret");
}

int task_create(void (*func)(), int priority, enum task_el mode){
  struct task *new_node = ll_pop_front<struct task>(&task_unuse);
  int pid = new_node->pid;
  new_node->state = RUNNING;
  new_node->priority = priority;
  new_node->counter = TASK_EPOCH;
  new_node->resched_flag = 0;
  new_node->invoke_func = func;
  new_node->mode = mode;
  new_node->pwd_vnode = get_root_vnode();
  for (int i=0; i<FD_MAX_NUM; i++){
    new_node->fd[i] = 0;
  }
  if (mode == KERNEL){
    new_node->lr = (unsigned long long)func;
  }
  else{
    void (*func_lr)() = user_task_start;
    new_node->lr = (unsigned long long)func_lr;
  }
  new_node->fp = (unsigned long long)(&kstack_pool[pid][STACK_TOP_IDX]);
  new_node->sp = (unsigned long long)(&kstack_pool[pid][STACK_TOP_IDX]);
  ll_push_back<struct task>(&task_queue_head[priority], new_node);
  return pid;
}

int privilege_task_create(void (*func)(), int priority){
  return task_create(func, priority, KERNEL);
}

int user_task_create(void (*func)(), int priority){
  return task_create(func, priority, USER);
}

void zombie_reaper(){
  for (int i=0; i<PRIORITY_MAX+1; i++){
    if (task_queue_head[i]){
      struct task *n = task_queue_head[i];
      while(n){
        if (n->state == ZOMBIE){
          char ct[20];
          int_to_str(n->pid, ct);
          log_puts((char * ) "\n[INFO] Reaper PID : ", INFO);
          log_puts(ct, INFO);
          log_puts((char * ) "\n", INFO);
          struct task *rm_task = n;
          n->state = EXIT;
          for (int i=0; i<FD_MAX_NUM; i++){
            if (n->fd[i]){
              do_close(i);
            }
          }
          n = n->next;
          ll_rm_elm<struct task>(&task_queue_head[i], rm_task);
          ll_push_front<struct task>(&task_unuse, rm_task);
        }
        else{
          n = n->next;
        }
      }
    }
  }
}

void yield(){
  struct task *current = get_current();
  current->resched_flag = 1;
  schedule();
}

void schedule(){
  zombie_reaper();
  struct task *current_task = get_current();
  struct task *next_task = get_current();
  for (int i=0; i<PRIORITY_MAX+1; i++){
    if (task_queue_head[i]){
      next_task = task_queue_head[i];
      break;
    }
  }
  if (current_task->pid == next_task->pid){
    if (current_task->resched_flag == 0){
      return ;
    }
    int priority = current_task->priority;
    ll_rm_elm<struct task>(&task_queue_head[priority], current_task);
    current_task->resched_flag = 0;
    current_task->counter = TASK_EPOCH;
    ll_push_back<struct task>(&task_queue_head[priority], current_task);
    next_task = task_queue_head[priority];
  }
  switch_to(current_task, next_task);
}

int get_pid(){
  struct task *c = get_current();
  return c->pid;
}

int get_new_fd(struct file *new_file){
  struct task *current = get_current();
  for (int i = 0; i<FD_MAX_NUM; i++){
    if (current->fd[i] == 0){
      current->fd[i] = new_file;
      return i;
    }
  }
  return -1;
}

struct file* get_file_by_fd(int fd){
  struct task *current = get_current();
  if (fd < 0 || fd > FD_MAX_NUM) return 0;
  return current->fd[fd];
}

void remove_fd(int fd){
  struct task *current = get_current();
  current->fd[fd] = 0;
}

void sys_fork(struct trapframe* trapframe){
  //char ct[20];
  IRQ_DISABLE();
  struct task* parent_task = get_current();

  int child_id = privilege_task_create(return_from_fork, parent_task->priority);
  //int child_id = privilege_task_create(0, parent_task->priority);
  struct task* child_task = &task_pool[child_id];

  char* child_kstack = &kstack_pool[child_task->pid][STACK_TOP_IDX];
  char* parent_kstack = &kstack_pool[parent_task->pid][STACK_TOP_IDX];
  char* child_ustack = &ustack_pool[child_task->pid][STACK_TOP_IDX];
  char* parent_ustack = &ustack_pool[parent_task->pid][STACK_TOP_IDX];

  unsigned long long kstack_offset = parent_kstack - (char*)trapframe;
  unsigned long long ustack_offset = parent_ustack - (char*)trapframe->sp_el0;

  for (unsigned long long i = 0; i < kstack_offset; i++) {
    *(child_kstack - i) = *(parent_kstack - i);
  }
  for (unsigned long long i = 0; i < ustack_offset; i++) {
    *(child_ustack - i) = *(parent_ustack - i);
  }

  // place child's kernel stack to right place
  child_task->sp = (unsigned long long)child_kstack - kstack_offset;

  child_task->pwd_vnode = parent_task->pwd_vnode;

  /*
  int_to_hex((unsigned long long) parent_ustack, ct);
  uart_puts(ct);
  uart_puts(", ");
  int_to_hex((unsigned long long) child_ustack, ct);
  uart_puts(ct);
  uart_puts("\n");
  */

  // place child's user stack to right place
  struct trapframe* child_trapframe = (struct trapframe*) child_task->sp;
  child_trapframe->sp_el0 = (unsigned long long)child_ustack - ustack_offset;

  /*
  child_trapframe->x[0] = 0;
  trapframe->x[0] = child_task->pid;
  int_to_hex((unsigned long long) trapframe->sp_el0, ct);
  uart_puts(ct);
  uart_puts(", ");
  int_to_hex((unsigned long long) child_trapframe->sp_el0, ct);
  uart_puts(ct);
  uart_puts("\n");
  int_to_hex((unsigned long long) parent_task->sp, ct);
  uart_puts(ct);
  uart_puts(", ");
  int_to_hex((unsigned long long) child_task->sp, ct);
  uart_puts(ct);
  uart_puts("\n");
  */
  //IRQ_ENABLE();
}

void sys_exec(struct trapframe *arg){
  void (*exec_func)() = (void(*)()) arg->x[0];
  char **input_argv = (char **) arg->x[1];
  int pid = get_pid();
  char* ustack = &ustack_pool[pid][STACK_TOP_IDX];

  int argc = 0;
  int argv_total_len = 0;
  while(input_argv[argc]){
    argv_total_len += str_len(input_argv[argc])+1;
    argc++;
  }
  char **argv = (char**)((unsigned long long)(ustack-argv_total_len) & (~15));
  argv--;
  *argv = 0;

  for (int i = argc-1; i>=0; i--){
    int strlen = str_len(input_argv[i]);
    ustack -= (strlen+1);
    argv--;
    *argv = ustack;
    str_copy(input_argv[i], ustack);
  }
  int ustack_offset = (unsigned long long) ustack%16;
  ustack -= ustack_offset;
  for (int i = 0; i<ustack_offset; i++){
    ustack[i] = '\0';
  }
  unsigned long long *argv_addr = (unsigned long long *)(argv-1);
  *argv_addr = (unsigned long long)argv;
  unsigned long long *argc_addr = (unsigned long long *)(argv-2);
  *argc_addr = argc;
  arg->sp_el0 = (unsigned long long)(argv-4);
  arg->elr_el1 = (unsigned long long) exec_func;
  arg->spsr_el1 = SPSR_EL1_VALUE;
  arg->x[1] = (unsigned long long) argv;
  arg->x[0] = argc;

}
