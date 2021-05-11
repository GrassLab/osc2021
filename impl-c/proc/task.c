#include "proc/task.h"
#include "proc.h"
#include "proc/argv.h"
#include "proc/exec.h"
#include "proc/sched.h"

#include "bool.h"
#include "list.h"
#include "mm.h"
#include "mm/frame.h"
#include "string.h"
#include "syscall.h"
#include "timer.h"
#include "uart.h"

#include "cfg.h"
#include "log.h"

#include <stdint.h>

#ifdef CFG_LOG_PROC_TASK
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

static void foo();

uint32_t new_tid = 0;

struct task_struct *task_create(void *func) {
  struct task_struct *t;
  t = (struct task_struct *)kalloc(FRAME_SIZE);
  if (t == NULL) {
    log_println("[task] oops cannot allocate thread");
    return NULL;
  }

  // Normal task is a kernel function, which has already been loaded to memory
  t->code = NULL;
  t->code_size = 0;
  t->cpu_context.fp = (uint64_t)t + FRAME_SIZE;
  t->cpu_context.lr = (uint64_t)func;
  t->cpu_context.sp = (uint64_t)t + FRAME_SIZE;
  t->status = TASK_STATUS_ALIVE;
  t->id = new_tid++;

  // A task would only bind to a user thread if called with exec_user
  t->user_stack = (uintptr_t)(NULL);
  t->user_sp = (uintptr_t)(NULL);

  struct task_entry *entry =
      (struct task_entry *)kalloc(sizeof(struct task_entry));
  entry->task = t;
  list_push(&entry->list, &run_queue);
  // uart_println("entry created: %x %x", entry->task, entry->list);

  log_println("task created: id:%d struct:%x func:%x", t->id, t,
              t->cpu_context.lr);
  return t;
}

int sys_getpid() {
  struct task_struct *task = get_current();
  return task->id;
};

// Overwrite current user task and kernel task
int sys_exec(const char *name, char *const args[]) {
  exec_user(name, args);
  // a scucess exec_user might never return
  return -1;
}

void task_copy(struct task_struct *dst, struct task_struct *src) {
  dst->status = src->status;
  dst->cpu_context = src->cpu_context;
  // copy the entire code: (could be optimized)
  if (src->code != NULL) {
    dst->code = (char *)kalloc(FRAME_SIZE);
    dst->code_size = src->code_size;
    memcpy(dst->code, src->code, src->code_size);
  }
}

int sys_fork() {
  struct task_struct *task = get_current();
  // store current context into user code

  // child task has been enqueued in to run_q
  struct task_struct *child = task_create(NULL);
  task_copy(child, task);

  // point to child Stack/Code
  intptr_t to_child_stack = (intptr_t)child - (intptr_t)task;
  intptr_t to_child_code = (intptr_t)child->code - (intptr_t)task->code;
  child->cpu_context.fp += to_child_stack;
  child->cpu_context.sp += to_child_stack;
  child->cpu_context.lr += to_child_code;

  // return child id
  return child->id;
}

// note: void exit();
void sys_exit() { cur_task_exit(); }

void cur_task_exit() {
  // Mark current thread as exited, which would be picked up by the idle thread
  struct task_struct *task = get_current();
  task->status = TASK_STATUS_DEAD;
  log_println("[task] exit called: %d", task->id);
  task_schedule();
}

void foo() {
  struct task_struct *task = get_current();
  for (int i = 0; i < 2; ++i) {
    uart_println("Thread id: %d -> loop:%d", task->id, i);
    _wait();
    task_schedule();
  }
  cur_task_exit();
}

// This function should be runned as a kernel task
void task_start_user() {
  uart_println("enter user startup");
  char *name = "./argv_test.out";
  char *args[4] = {"./argv_test.out", "-o", "arg2", NULL};

  // Launch a user program thread (in el0) with this kernel thread
  //
  // Eversince this point, exceptions under el0 would be trap
  // to the context of this kernel thread
  // (Since we would call `eret` under this kernel thread)
  exec_user(name, args);
}

void test_tasks() {
  proc_init();

  struct task_struct *root_task;
  root_task = task_create(idle);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((uint64_t)root_task));

  // create a task to bootup the very first user program
  task_create(task_start_user);

  task_create(foo);
  // task_create(foo);
  // task_create(foo);
#ifdef CFG_LOG_PROC_SCHED
  _dump_runq();
#endif
  idle();
}