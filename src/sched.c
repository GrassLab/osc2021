#include "sched.h"

#include "exc.h"
#include "io.h"
#include "mem.h"
#include "timer.h"
#include "util.h"

cdl_list run_queue;
cdl_list dead_queue;

unsigned long pid_cnt;

typedef struct task_struct {
  cdl_list task_list;
  void *sp;
  unsigned long pid;
  unsigned long st_timer_cnt;
  unsigned long exc_lvl;
  void *user_sp;
  unsigned long user_sp_size;
} task_struct;

typedef struct pres_reg {
  unsigned long x29, x30, x27, x28, x25, x26, x23, x24, x21, x22, x19, x20;
} pres_reg;

#define sp_low(sp)                                           \
  ((void *)((((unsigned long)sp - 1) >> KERN_STACK_SIZE_CTZ) \
            << KERN_STACK_SIZE_CTZ))

#define sp_high(sp) (sp_low(sp) + KERN_STACK_SIZE)

#define sp_to_ts(sp) ((task_struct *)sp_low(sp))

unsigned long preemt_sched = 0;
unsigned long time_slice;

void init_sched() {
  disable_interrupt();
  init_cdl_list(&run_queue);
  init_cdl_list(&dead_queue);
  pid_cnt = 0;
  enable_interrupt();
  void *sp = get_sp();
  unsigned long pid = get_new_pid();
  task_struct *ts = sp_to_ts(sp);
  set_pid(pid);
  ts->pid = pid;
  ts->exc_lvl = 0;
  ts->st_timer_cnt = get_timer_cnt();
  ts->user_sp = NULL;
  ts->user_sp_size = 0;
  time_slice = timer_frq / 1024;
  preemt_sched = 1;
}

void _schedule(task_struct *ts) {
  task_struct *new_ts = pop_cdl_list(run_queue.fd);
  unsigned long int_stat = get_int_stat();
  switch_exec(new_ts->sp, new_ts->pid, &(ts->sp));
  set_int_stat(int_stat);
  clear_task_timer();
}

void schedule() {
  void *sp = get_sp();
  task_struct *ts = sp_to_ts(sp);
  disable_interrupt();
  push_back_cdl_list(&run_queue, &(ts->task_list));
  _schedule(ts);
  enable_interrupt();
}

void die() {
  void *sp = get_sp();
  task_struct *ts = sp_to_ts(sp);
  log_hex("die", ts->pid, LOG_PRINT);
  disable_interrupt();
  push_back_cdl_list(&dead_queue, &(ts->task_list));
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
  disable_interrupt();
  push_back_cdl_list(&run_queue, &(ts->task_list));
  enable_interrupt();
}

void idle() {
  while (1) {
    reap_dead(&dead_queue);
    schedule();
  }
}

void reap_dead(cdl_list *reap_list) {
  while (!cdl_list_empty(reap_list)) {
    task_struct *ts = pop_cdl_list(reap_list->fd);
    log_hex("reap", ts->pid, LOG_PRINT);
    kfree((void *)ts);
  }
}

void wake_up(task_struct *ts) {
  disable_interrupt();
  push_back_cdl_list(&run_queue, &(ts->task_list));
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

  memcpy_ul(new_ts, ts, sizeof(ts));
  new_ts->pid = get_new_pid();
  memcpy_ul(new_sp, sp, sp_size);

  disable_interrupt();
  push_back_cdl_list(&run_queue, &(new_ts->task_list));
  push_back_cdl_list(&run_queue, &(ts->task_list));
  task_struct *next_ts = pop_cdl_list(run_queue.fd);
  unsigned long int_stat = get_int_stat();
  switch_exec_clone(next_ts->sp, next_ts->pid, &(ts->sp), new_sp, &(new_ts->sp));
  set_int_stat(int_stat);
  clear_task_timer();
  enable_interrupt();

  if(get_pid() == ts->pid) {
    return new_ts->pid;
  } else {
    return 0;
  }
}

void exc_lvl_inc() {
  task_struct *ts = sp_to_ts((get_sp()));
  ts->exc_lvl++;
}

void exc_lvl_consume() {
  task_struct *ts = sp_to_ts((get_sp()));
  ts->exc_lvl--;
  if(ts->exc_lvl == 0) {
    if(get_timer_cnt() - ts->st_timer_cnt > time_slice) {
      schedule();
    }
  }
}

void clear_task_timer() {
  task_struct *ts = sp_to_ts(get_sp());
  ts->st_timer_cnt = get_timer_cnt();
}