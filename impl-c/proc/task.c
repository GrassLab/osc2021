#include "proc/task.h"
#include "bool.h"
#include "list.h"
#include "mm.h"
#include "mm/frame.h"
#include "proc.h"
#include "proc/sched.h"
#include "syscall.h"
#include "uart.h"

#include "cfg.h"
#include "log.h"

#ifdef CFG_LOG_PROC_TASK
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

static void foo();

void foo() {
  struct task_struct *task = get_current();
  for (int i = 0; i < 2; ++i) {
    uart_println("Thread id: %d -> loop:%d", task->id, i);
    _wait();
    task_schedule();
  }
  cur_task_exit();
}

struct task_struct *task_create(void *func, int tid) {
  struct task_struct *t;
  t = (struct task_struct *)kalloc(FRAME_SIZE);
  if (t == NULL) {
    log_println("[task] oops cannot allocate thread");
    return NULL;
  }

  t->cpu_context.fp = (unsigned long)t + FRAME_SIZE;
  t->cpu_context.lr = (unsigned long)func;
  t->cpu_context.sp = (unsigned long)t + FRAME_SIZE;
  t->status = TASK_STATUS_ALIVE;
  t->id = tid;

  struct task_entry *entry =
      (struct task_entry *)kalloc(sizeof(struct task_entry));
  entry->task = t;
  list_push(&entry->list, &run_queue);
  // uart_println("entry created: %x %x", entry->task, entry->list);

  log_println("task created: id:%d struct:%x func:%x", t->id, t,
              t->cpu_context.lr);
  return t;
}

void sys_getpid(struct trap_frame *tf) {
  // dummy version
  uart_println("pid: %d", 87);
  tf->regs[0] = 87;
};

void cur_task_exit() {
  // Exit current running thread;
  struct task_struct *task = get_current();
  task->status = TASK_STATUS_DEAD;
  log_println("[task] exit called: %d", task->id);
  task_schedule();
}

void test_tasks() {
  proc_init();

  struct task_struct *root_task;
  root_task = task_create(idle, 0);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((unsigned long)root_task));

  task_create(foo, 1);
  task_create(foo, 2);
  task_create(foo, 3);
#ifdef CFG_LOG_PROC_SCHED
  _dump_runq();
#endif
  idle();
}