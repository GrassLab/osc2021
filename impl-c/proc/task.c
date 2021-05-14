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

extern void fork_child_eret();

uint32_t new_tid = 0;

struct task_struct *task_create(void *func) {
  struct task_struct *t;
  t = (struct task_struct *)kalloc(sizeof(struct task_struct));
  if (t == NULL) {
    log_println("[task] oops cannot allocate thread");
    return NULL;
  }

  t->kernel_stack = (uintptr_t)kalloc(FRAME_SIZE);

  // Normal task is a kernel function, which has already been loaded to memory
  t->code = NULL;
  t->code_size = 0;
  t->cpu_context.fp = t->kernel_stack + FRAME_SIZE;
  t->cpu_context.lr = (uint64_t)func;
  t->cpu_context.sp = t->kernel_stack + FRAME_SIZE;
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

int sys_fork(const struct trap_frame *tf) {

  struct task_struct *parent = get_current();

  {
    // just logging
    uintptr_t el1_sp;
    asm volatile("mov %0, sp" : "=r"(el1_sp));
    log_println("current sp(sp_el1): %x", el1_sp);
  }

  struct task_struct *child = task_create(NULL);

  // USER STACK
  child->user_stack = (uintptr_t)kalloc(FRAME_SIZE);
  memcpy((char *)child->user_stack, (const char *)parent->user_stack,
         FRAME_SIZE);

  // KERNEL STACK
  memcpy((char *)child->kernel_stack, (const char *)parent->kernel_stack,
         FRAME_SIZE);

  // CODE
  // TODO: fix this (memory leak)
  // parent should free memory
  // parent should be collected after child
  child->code = NULL;

  // Child return
  // direct to the exception return point
  uintptr_t kstack_tf_offset = ((uintptr_t)tf) - parent->kernel_stack;
  struct trap_frame *child_tf =
      (struct trap_frame *)(child->kernel_stack + kstack_tf_offset);
  child->cpu_context.sp = (uintptr_t)child_tf;
  child->cpu_context.lr = (uint64_t)fork_child_eret;
  child_tf->regs[0] = 0;

  log_println("parent kstack:%x ustack:%x", parent->kernel_stack,
              parent->user_stack);
  log_println("   tf:%x ctx.fp:%x ctx.sp:%x ctx.lr: %x", tf,
              parent->cpu_context.fp, parent->cpu_context.sp,
              parent->cpu_context.lr);

  log_println("child kstack:%x ustack:%x ", child->kernel_stack,
              child->user_stack);
  log_println("   tf:%x ctx.fp:%x ctx.sp:%x ctx.lr: %x", child_tf,
              child->cpu_context.fp, child->cpu_context.sp,
              child->cpu_context.lr);

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