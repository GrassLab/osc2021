#ifndef SCHED_H
#define SCHED_H

#include "mem.h"
#include "trap.h"
#include "util.h"
#include "vfs.h"

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
  dentry *pwd;
  cdl_list fd_list;
  unsigned long fd_cnt;
} task_struct;

#define KERN_STACK_SIZE (PAGE_SIZE * 2)
#define KERN_STACK_SIZE_CTZ (PAGE_SIZE_CTZ + 1)
#define USR_STACK_SIZE (PAGE_SIZE * 2)
extern unsigned long preemt_sched;

#define sp_low(sp)                                           \
  ((void *)((((unsigned long)(sp)-1) >> KERN_STACK_SIZE_CTZ) \
            << KERN_STACK_SIZE_CTZ))

#define sp_high(sp) (sp_low(sp) + KERN_STACK_SIZE)

#define sp_to_ts(sp) ((task_struct *)sp_low(sp))

void init_sched();
void schedule();
unsigned long get_new_pid();
void thread_create(void *func);
void idle();
void die();
void reap_dead();
unsigned long clone();
void sleep(double sec);
void exc_lvl_gain();
void exc_lvl_consume();
void clear_task_timer();
unsigned long wait();

unsigned long get_pid();
void set_pid(unsigned long);
void *get_sp();
void thread_start();
void switch_exec(void **new_sp, unsigned long new_pid, void **old_sp);
void switch_exec_clone(void **new_sp, unsigned long new_pid, void **old_sp1,
                       void **old_sp2);

void execve(const char *filename, char *const argv[]);
void exec(const char *path);
void execve_refresh_stack(void *usr_prog, void *usr_sp, void *sp);
void _execve(void *usr_prog, void *usr_sp, void *sp);

void wait_on_list(cdl_list *l);
void wake_list(cdl_list *l);

void fork(void **el0_sp, saved_reg **el1_sp);

static inline task_struct *get_taskstruct() { return sp_to_ts(get_sp()); }

#endif