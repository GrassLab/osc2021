# include "schedule.h"
# include "shell.h"
# include "uart.h"
# include "my_math.h"
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
}

void task_exit(){
  struct task *current = get_current();
  current->state = ZOMBIE;
  schedule();
}

void task_start(){
  struct task *current = get_current();
  if (current->mode == KERNEL){
    void (*func)() = current->invoke_func;
    func();
    task_exit();
  }
  else if(current->mode == USER){;
  }
  task_exit();
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
  void (*func_lr)() = task_start;
  new_node->lr = (unsigned long long)func_lr;
  new_node->fp = (unsigned long long)(&kstack_pool[pid][STACK_TOP_IDX]);
  new_node->sp = (unsigned long long)(&kstack_pool[pid][STACK_TOP_IDX]);
  ll_push_back<struct task>(&task_queue_head[priority], new_node);
  return pid;
}

int privilege_task_create(void (*func)(), int priority){
  return task_create(func, priority, KERNEL);
}

void zombie_reaper(){
  for (int i=0; i<PRIORITY_MAX+1; i++){
    if (task_queue_head[i]){
      struct task *n = task_queue_head[i];
      while(n){
        if (n->state == ZOMBIE){
          char ct[20];
          int_to_str(n->pid, ct);
          uart_puts((char * ) "Reaper PID : ");
          uart_puts(ct);
          uart_puts((char * ) "\n");
          struct task *rm_task = n;
          n->state = EXIT;
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
