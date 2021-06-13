# include "schedule.h"
# include "exception.h"
# include "shell.h"
# include "uart.h"
# include "my_math.h"
# include "my_string.h"
# include "page.h"
# include "start.h"
# include "vfs.h"
# include "mem.h"
# include "log.h"
# include "mem_addr.h"
# include "linklist.c"
# include "fat32fs.h"
# include "flags.h"


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

int load_app(int fd, uint64_t ttbr0){
  uint64_t v_addr = USER_PRO_LR_START;
  int readbytes;
  do{
    uint64_t read_addr = create_user_page(v_addr, ttbr0);
    char *readbuf = (char*) (read_addr | KVA);
    readbytes = do_read(fd, readbuf, SECTOR_SIZE);
    char ct[20];
    int_to_str(readbytes, ct);
    log_puts("[INFO] Load ", FINE);
    log_puts(ct, FINE);
    log_puts(" bytes to ", FINE);
    int_to_hex( ((uint64_t)readbuf) & (~KVA), ct);
    log_puts(ct, FINE);
    log_puts("\n", FINE);
    v_addr += SECTOR_SIZE;
  } while(readbytes == SECTOR_SIZE);
  log_puts("\n", FINE);
  create_user_page(USER_PRO_SP_START, ttbr0);
  return 0;
}

void user_task_start(){
  asm volatile("msr sp_el0, %0" : : "r"(USER_PRO_SP_START));
  asm volatile("msr elr_el1, %0": : "r"(USER_PRO_LR_START));
  asm volatile("msr spsr_el1, %0" : : "r"(SPSR_EL1_VALUE));
  asm volatile("eret");
}

int task_create(void (*func)(), int priority, enum task_el mode, int ifttbr0){
  struct task *new_node = ll_pop_front<struct task>(&task_unuse);
  int pid = new_node->pid;
  new_node->state = RUNNING;
  new_node->priority = priority;
  new_node->counter = TASK_EPOCH;
  new_node->resched_flag = 0;
  new_node->mode = mode;
  new_node->pwd_vnode = get_root_vnode();
  for (int i=0; i<FD_MAX_NUM; i++){
    new_node->fd[i] = 0;
  }
  new_node->lr = (unsigned long long)func;
  new_node->ttbr0 = (ifttbr0) ? ((unsigned long long) malloc(PAGE_SIZE, 1)) : 0;
  new_node->fp = (unsigned long long)(&kstack_pool[pid][STACK_TOP_IDX]);
  new_node->sp = (unsigned long long)(&kstack_pool[pid][STACK_TOP_IDX]);
  ll_push_back<struct task>(&task_queue_head[priority], new_node);
  return pid;
}

int privilege_task_create(void (*func)(), int priority){
  return task_create(func, priority, KERNEL, 0);
}

int user_task_create(char *pathname, int priority){
  int fd = do_open(pathname, O_RD);
  if (fd < 0) {
    uart_puts((char *) "Path <");
    uart_puts(pathname);
    uart_puts((char *) "> not found.\n");
    return -1;
  }
  IRQ_DISABLE();
  int pid = task_create(user_task_start, priority, USER, 1);
  load_app(fd, task_pool[pid].ttbr0);
  LOG(FINE) user_pt_show((void*) task_pool[pid].ttbr0);
  do_close(fd);
  IRQ_ENABLE();
  return pid;
}

int fork_task_create(int priority){
  return task_create(return_from_fork, priority, USER, 1);
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
          log_puts((char * ) "\n\n", INFO);
          struct task *rm_task = n;
          n->state = EXIT;
          for (int i=0; i<FD_MAX_NUM; i++){
            if (n->fd[i]){
              do_close(i);
            }
          }
          if (n->ttbr0) {
            rmall_user_page(n->ttbr0);
            free((void*) n->ttbr0);
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
  update_pgd(next_task->ttbr0 & (~KVA));
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
  IRQ_DISABLE();
  struct task* parent_task = get_current();

  int child_id = fork_task_create(parent_task->priority);
  struct task* child_task = &task_pool[child_id];

  char* child_kstack = &kstack_pool[child_task->pid][STACK_TOP_IDX];
  char* parent_kstack = &kstack_pool[parent_task->pid][STACK_TOP_IDX];

  unsigned long long kstack_offset = parent_kstack - (char*)trapframe;
  
  
  for (unsigned long long i = 0; i < kstack_offset; i++) {
    *(child_kstack - i) = *(parent_kstack - i);
  }

  child_task->sp = (unsigned long long)child_kstack - kstack_offset;

  child_task->pwd_vnode = parent_task->pwd_vnode;
  child_task->mode = parent_task->mode;
  
  // place child's user stack to right place
  struct trapframe* child_trapframe = (struct trapframe*) child_task->sp;
  child_trapframe->sp_el0 = trapframe->sp_el0;
  
  if (parent_task->ttbr0){
    list_head parent_ttbr_head;
    list_head_init(&parent_ttbr_head);
    getall_user_page((void*) (parent_task->ttbr0 | KVA), &parent_ttbr_head);
    list_head *pos;
    list_for_each(pos, &parent_ttbr_head){
      struct pg_list *t = container_of(pos, struct pg_list, head);
      int64_t child_pa = create_user_page(t->va, child_task->ttbr0);
      memcpy((void*)(t->pa | KVA), (void*)(child_pa | KVA), PAGE_SIZE);
    }
    LOG(FINE){
      user_pt_show((void*) parent_task->ttbr0);
      user_pt_show((void*) child_task->ttbr0);
    }
  }

  child_trapframe->x[0] = 0;
  trapframe->x[0] = child_task->pid;
  IRQ_ENABLE();
}

static void* exec_pa_to_va(void* pa, void* pa_base, void* va_base){
  uint64_t offset = ((uint64_t)pa_base)-((uint64_t)va_base);
  return (void*)((uint64_t)pa-offset);
}

void sys_exec(struct trapframe *arg){
  char *exec_file = (char*) arg->x[0];
  char **input_argv = (char **) arg->x[1];
  
  int fd = do_open(exec_file, O_RD);
  if (fd < 0) {
    uart_puts((char *) "[Error] Exec's  path <");
    uart_puts(exec_file);
    uart_puts((char *) "> not found.\n");
    arg->x[0] = -1;
    return ;
  }
  int pid = get_pid();
  uint64_t new_ttbr0 = (uint64_t) malloc(PAGE_SIZE, 1);
  load_app(fd, new_ttbr0);
  do_close(fd);
  LOG(FINE) user_pt_show((void*) new_ttbr0);
  for (int i=0; i<FD_MAX_NUM; i++){
    task_pool[pid].fd[i] = 0;
  }
  
  char* new_ustack_top = (char*) (va_to_pa( USER_PRO_SP_START, (void*)new_ttbr0) | KVA);
  char* ustack = new_ustack_top;
  
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
    *argv = (char*) exec_pa_to_va(ustack, new_ustack_top, (void*)USER_PRO_SP_START);
    str_copy(input_argv[i], ustack);
  }
  
  int ustack_offset = (unsigned long long) ustack%16;
  ustack -= ustack_offset;
  for (int i = 0; i<ustack_offset; i++){
    ustack[i] = '\0';
  }
  
  arg->sp_el0 = (unsigned long long) exec_pa_to_va(argv-2, new_ustack_top, (void*)USER_PRO_SP_START);
  arg->elr_el1 = (unsigned long long) USER_PRO_LR_START;
  arg->spsr_el1 = SPSR_EL1_VALUE;
  arg->x[1] = (unsigned long long) exec_pa_to_va(argv, new_ustack_top, (void*)USER_PRO_SP_START);
  arg->x[0] = argc;
  
  rmall_user_page(task_pool[pid].ttbr0);
  free((void*)task_pool[pid].ttbr0);
  task_pool[pid].ttbr0 = new_ttbr0;
  update_pgd(new_ttbr0);
}
