#include "sched.h"

#include "cpio.h"
#include "exc.h"
#include "io.h"
#include "mem.h"
#include "timer.h"
#include "trap.h"
#include "util.h"

cdl_list run_queue;
unsigned long pid_cnt;

typedef struct mem_seg {
  // low address
  void *base;
  // real size (copy use)
  unsigned long size;
  unsigned long ref_cnt;
} mem_seg;

typedef struct task_struct {
  cdl_list ts_list;
  cdl_list sibli;
  cdl_list child;
  cdl_list dead;
  struct task_struct *parent;
  void *sp;
  unsigned long pid;
  unsigned long st_timer_cnt;
  unsigned long exc_lvl;
  mem_seg *usr_sp;
  mem_seg *usr_prog;
} task_struct;

typedef struct pres_reg {
  unsigned long x29, x30, x27, x28, x25, x26, x23, x24, x21, x22, x19, x20;
} pres_reg;

#define sp_low(sp)                                           \
  ((void *)((((unsigned long)(sp)-1) >> KERN_STACK_SIZE_CTZ) \
            << KERN_STACK_SIZE_CTZ))

#define sp_high(sp) (sp_low(sp) + KERN_STACK_SIZE)

#define sp_to_ts(sp) ((task_struct *)sp_low(sp))

unsigned long preemt_sched = 0;
unsigned long time_slice;

void init_sched() {
  disable_interrupt();
  init_cdl_list(&run_queue);
  pid_cnt = 0;
  enable_interrupt();
  unsigned long pid = get_new_pid();
  task_struct *ts = sp_to_ts(get_sp());
  set_pid(pid);
  ts->pid = pid;
  ts->exc_lvl = 0;
  ts->st_timer_cnt = get_timer_cnt();
  ts->usr_sp = NULL;
  ts->usr_prog = NULL;
  init_cdl_list(&(ts->sibli));
  init_cdl_list(&(ts->child));
  init_cdl_list(&(ts->dead));
  ts->parent = NULL;
  time_slice = timer_frq / 1024;
  preemt_sched = 1;
}

void _schedule(task_struct *ts) {
  task_struct *new_ts = pop_cdl_list(run_queue.fd);
  unsigned long int_stat = get_int_stat();
  switch_exec(&(new_ts->sp), new_ts->pid, &(ts->sp));
  set_int_stat(int_stat);
  clear_task_timer();
}

void schedule() {
  task_struct *ts = sp_to_ts(get_sp());
  disable_interrupt();
  push_back_cdl_list(&run_queue, &(ts->ts_list));
  _schedule(ts);
  enable_interrupt();
}

void die() {
  task_struct *ts = sp_to_ts(get_sp());
  log_hex("die", ts->pid, LOG_PRINT);
  disable_interrupt();
  if (ts->usr_sp != NULL) {
    ts->usr_sp->ref_cnt--;
    if (ts->usr_sp->ref_cnt == 0) {
      kfree(ts->usr_sp->base);
      kfree(ts->usr_sp);
    }
    ts->usr_sp = NULL;
  }
  enable_interrupt();
  disable_interrupt();
  if (ts->usr_prog != NULL) {
    ts->usr_prog->ref_cnt--;
    if (ts->usr_prog->ref_cnt == 0) {
      kfree(ts->usr_prog->base);
      kfree(ts->usr_prog);
    }
    ts->usr_prog = NULL;
  }
  enable_interrupt();
  // reparenting
  disable_interrupt();
  cdl_list *child_itr = ts->child.fd;
  while (child_itr != &(ts->child)) {
    task_struct *child_ts = sp_to_ts(child_itr);
    if (child_ts->parent != ts) {
      log("crb", LOG_ERROR);
    }
    child_ts->parent = ts->parent;
    child_itr = child_itr->fd;
  }
  concat_cdl_list(&(ts->parent->child), &(ts->child));
  concat_cdl_list(&(ts->parent->dead), &(ts->dead));
  pop_cdl_list(&(ts->sibli));
  push_cdl_list(&(ts->parent->dead), &(ts->ts_list));
  _schedule(ts);
}

unsigned long get_new_pid() {
  disable_interrupt();
  unsigned long pid = ++pid_cnt;
  enable_interrupt();
  return pid;
}

void thread_create(void *func) {
  void *sp = kmalloc(KERN_STACK_SIZE) + KERN_STACK_SIZE - sizeof(pres_reg);
  task_struct *ts = sp_to_ts(sp);
  memset_ul(sp, 0, sizeof(pres_reg));
  ((pres_reg *)sp)->x30 = (unsigned long)&thread_start;
  ((pres_reg *)sp)->x20 = (unsigned long)func;
  ts->pid = get_new_pid();
  ts->sp = sp;
  ts->exc_lvl = 0;
  ts->usr_sp = NULL;
  ts->usr_prog = NULL;
  task_struct *pts = sp_to_ts(get_sp());
  ts->parent = pts;
  disable_interrupt();
  push_cdl_list(&(pts->child), &(ts->sibli));
  enable_interrupt();
  init_cdl_list(&(ts->child));
  init_cdl_list(&(ts->dead));
  disable_interrupt();
  push_back_cdl_list(&run_queue, &(ts->ts_list));
  enable_interrupt();
}

void idle() {
  while (1) {
    reap_dead();
    schedule();
  }
}

void reap_dead() {
  cdl_list *reap_list = &(sp_to_ts(get_sp())->dead);
  while (!cdl_list_empty(reap_list)) {
    task_struct *ts = pop_cdl_list(reap_list->fd);
    log_hex("rip", ts->pid, LOG_PRINT);
    kfree((void *)ts);
  }
}

void wake_up(task_struct *ts) {
  disable_interrupt();
  push_back_cdl_list(&run_queue, &(ts->ts_list));
  enable_interrupt();
}

void sleep(double sec) {
  task_struct *ts = sp_to_ts(get_sp());
  add_timer(sec, &wake_up, (void *)ts);
  disable_interrupt();
  _schedule(ts);
  enable_interrupt();
}

unsigned long clone() {
  void *sp = get_sp();
  unsigned long sp_size = sp_high(sp) - sp;
  task_struct *ts = sp_to_ts(sp);

  void *new_sp = kmalloc(KERN_STACK_SIZE) + KERN_STACK_SIZE - sp_size;
  task_struct *new_ts = sp_to_ts(new_sp);

  disable_interrupt();
  if (ts->usr_sp != NULL) {
    ts->usr_sp->ref_cnt++;
  }
  if (ts->usr_prog != NULL) {
    ts->usr_prog->ref_cnt++;
  }
  enable_interrupt();
  new_ts->usr_sp = ts->usr_sp;
  new_ts->usr_prog = ts->usr_prog;
  new_ts->pid = get_new_pid();
  new_ts->parent = ts;
  new_ts->exc_lvl = ts->exc_lvl;
  new_ts->sp = new_sp;
  init_cdl_list(&(new_ts->child));
  init_cdl_list(&(new_ts->dead));
  disable_interrupt();
  push_cdl_list(&(ts->child), &(new_ts->sibli));
  enable_interrupt();
  unsigned long int_stat = get_int_stat();
  memcpy_ul(new_sp, sp, sp_size);

  disable_interrupt();
  push_back_cdl_list(&run_queue, &(new_ts->ts_list));
  push_back_cdl_list(&run_queue, &(ts->ts_list));
  task_struct *next_ts = pop_cdl_list(run_queue.fd);
  switch_exec_clone(&(next_ts->sp), next_ts->pid, &(ts->sp), &(new_ts->sp));
  set_int_stat(int_stat + 1);
  clear_task_timer();
  enable_interrupt();

  if (get_pid() == ts->pid) {
    return new_ts->pid;
  } else {
    return 0;
  }
}

void exc_lvl_gain() {
  task_struct *ts = sp_to_ts((get_sp()));
  ts->exc_lvl++;
}

void exc_lvl_consume() {
  task_struct *ts = sp_to_ts((get_sp()));
  ts->exc_lvl--;
  if (preemt_sched == 1 && ts->exc_lvl == 0) {
    if (get_timer_cnt() - ts->st_timer_cnt > time_slice) {
      schedule();
    }
  }
}

void clear_task_timer() {
  task_struct *ts = sp_to_ts(get_sp());
  ts->st_timer_cnt = get_timer_cnt();
}

unsigned long wait() {
  cdl_list *reap_list = &(sp_to_ts(get_sp())->dead);
  while (cdl_list_empty(reap_list)) {
    schedule();
  }
  task_struct *ts = pop_cdl_list(reap_list->fd);
  unsigned long pid = ts->pid;
  log_hex("rip", pid, LOG_PRINT);
  kfree((void *)ts);
  return pid;
}

typedef struct arg_stack {
  unsigned long argc;
  char **argv;
  char *args[2];
} arg_stack;

mem_seg *get_usr_prog(const char *fn) {
  void *cpio_file = get_cpio_file(fn);
  if (cpio_file != NULL) {
    unsigned long file_size = get_file_size(cpio_file);
    void *file_data = get_file_data(cpio_file);
    void *usr_prog = kmalloc(pad(file_size, 4096));
    memcpy(usr_prog, file_data, file_size);
    mem_seg *prog_seg = kmalloc(sizeof(mem_seg));
    prog_seg->base = usr_prog;
    prog_seg->ref_cnt = 1;
    prog_seg->size = file_size;
    return prog_seg;
  } else {
    print("program not found\n");
    return NULL;
  }
}

mem_seg *get_usr_stack(char *const argv[]) {
  void *user_sp = kmalloc(USR_STACK_SIZE) + USR_STACK_SIZE;
  int i = 0;
  unsigned long argv_total_size = 0;
  while (argv[i] != NULL) {
    argv_total_size += (strlen(argv[i]) + 1);
    i++;
  }
  argv_total_size = pad(argv_total_size, 8);
  unsigned long sp_size = argv_total_size + 16 + 8 * (i + 1);
  sp_size = pad(sp_size, 16);
  arg_stack *as = (arg_stack *)(user_sp - sp_size);
  as->argc = i;
  as->argv = &(as->args[0]);
  char *argv_itr = user_sp - argv_total_size;
  for (int j = 0; j < i; j++) {
    as->args[j] = argv_itr;
    strcpy(argv_itr, argv[j]);
    argv_itr += (strlen(argv[j]) + 1);
  }
  as->args[i] = NULL;
  mem_seg *sp_seg = kmalloc(sizeof(mem_seg));
  sp_seg->base = user_sp - USR_STACK_SIZE;
  sp_seg->size = sp_size;
  sp_seg->ref_cnt = 1;
  return sp_seg;
}

void execve(const char *filename, char *const argv[]) {
  mem_seg *prog_seg = get_usr_prog(filename);
  if (prog_seg == NULL) {
    die();
  }
  mem_seg *sp_seg = get_usr_stack(argv);
  void *sp = sp_high(get_sp());
  execve_refresh_stack(prog_seg, sp_seg, sp);
}

void exec(const char *path) {
  char *p = new_str(path);
  unsigned long arg_cnt = cnt_white(path) + 1;
  char **argv = kmalloc(sizeof(char *) * (arg_cnt + 1));
  char *p_itr = p;
  for (int i = 0; i < arg_cnt; i++) {
    argv[i] = p_itr;
    p_itr = split_str(p_itr);
  }
  argv[arg_cnt] = NULL;

  mem_seg *prog_seg = get_usr_prog(argv[0]);
  if (prog_seg == NULL) {
    kfree((void *)argv);
    kfree((void *)p);
    die();
  }
  mem_seg *sp_seg = get_usr_stack(&(argv[0]));
  void *sp = sp_high(get_sp());
  kfree((void *)argv);
  kfree((void *)p);
  execve_refresh_stack(prog_seg, sp_seg, sp);
}

void _execve(void *usr_prog, void *usr_sp, void *sp) {
  mem_seg *prog = usr_prog;
  mem_seg *stack = usr_sp;
  saved_reg *sr = sp;
  task_struct *ts = sp_to_ts(sp);
  disable_interrupt();
  if (ts->usr_sp != NULL) {
    ts->usr_sp->ref_cnt--;
    if (ts->usr_sp->ref_cnt == 0) {
      kfree(ts->usr_sp->base);
      kfree(ts->usr_sp);
    }
    ts->usr_sp = NULL;
  }
  enable_interrupt();
  disable_interrupt();
  if (ts->usr_prog != NULL) {
    ts->usr_prog->ref_cnt--;
    if (ts->usr_prog->ref_cnt == 0) {
      kfree(ts->usr_prog->base);
      kfree(ts->usr_prog);
    }
    ts->usr_prog = NULL;
  }
  enable_interrupt();
  ts->usr_prog = prog;
  ts->usr_sp = stack;
  ts->exc_lvl = 0;
  memset_ul(sr, 0, sizeof(saved_reg));
  disable_interrupt();
  set_int_stat(0);
  ret_exc(prog->base, stack->base + USR_STACK_SIZE - stack->size, 0, sp);
}

void wait_on_list(cdl_list *l) {
  task_struct *ts = sp_to_ts(get_sp());
  disable_interrupt();
  push_cdl_list(l, &(ts->ts_list));
  _schedule(ts);
  enable_interrupt();
}
void wake_list(cdl_list *l) {
  while (!cdl_list_empty(l)) {
    disable_interrupt();
    cdl_list *t = pop_cdl_list(l->fd);
    push_back_cdl_list(&run_queue, t);
    enable_interrupt();
  }
}

void fork(void **el0_sp, saved_reg **el1_sp) {
  void *sp = get_sp();
  task_struct *ts = sp_to_ts(sp);
  void *sp_base = sp_low(sp);
  unsigned long el0_sp_s = ((void *)el0_sp) - sp_base;
  unsigned long el1_sp_s = ((void *)el1_sp) - sp_base;
  unsigned long el1_sp_ss = (*(void **)el1_sp) - sp_base;

  mem_seg *new_sp_seg = NULL;
  mem_seg *sp_seg = ts->usr_sp;
  if (sp_seg != NULL) {
    new_sp_seg = kmalloc(sizeof(mem_seg));
    new_sp_seg->base = kmalloc(USR_STACK_SIZE);
    new_sp_seg->ref_cnt = 1;
    new_sp_seg->size = sp_seg->base + USR_STACK_SIZE - *el0_sp;
    memcpy_ul(new_sp_seg->base + USR_STACK_SIZE - new_sp_seg->size, *el0_sp,
              new_sp_seg->size);
  }
  unsigned long pid = clone();
  if (pid == 0) {
    void *csp = get_sp();
    task_struct *cts = sp_to_ts(csp);
    disable_interrupt();
    if (sp_seg != NULL) {
      sp_seg->ref_cnt--;
      if (sp_seg->ref_cnt == 0) {
        kfree(sp_seg->base);
        kfree(sp_seg);
      }
    }
    enable_interrupt();
    cts->usr_sp = new_sp_seg;
    sp_base = sp_low(csp);
    el0_sp = (void **)(el0_sp_s + sp_base);
    *el0_sp = cts->usr_sp->base + USR_STACK_SIZE - cts->usr_sp->size;
    el1_sp = (saved_reg **)(el1_sp_s + sp_base);
    *el1_sp = (saved_reg *)(el1_sp_ss + sp_base);
  }
  (*el1_sp)->x0 = pid;
}