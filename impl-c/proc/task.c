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

// Overwrite current task
int sys_exec(const char *name, char *const args[]) { return exec(name, args); }

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

// This funciton is purely user code
void user_startup() {
  uart_println("enter user startup");
  const char *name = "./init_user.out";
  struct task_struct *task = get_current();
  // address of the program code in memory
  void *entry_point = load_program(name, &task->code_size);
  uart_println("program loaded: %x", entry_point);

  task->code = entry_point;

  task->cpu_context.fp = (uint64_t)task + FRAME_SIZE;
  task->cpu_context.lr = (uint64_t)entry_point;
  task->cpu_context.sp = (uint64_t)task + FRAME_SIZE;

  uart_println("finished, start switching");
  asm volatile("mov x0, 0x340  \n"); // enable core timer interrupt
  asm volatile("msr spsr_el1, x0  \n");
  asm volatile("msr elr_el1, %0   \n" ::"r"(task->cpu_context.lr));
  asm volatile("msr sp_el0, %0    \n" ::"r"(task->cpu_context.sp));

  // enable the core timer’s interrupt in el0
  timer_el0_enable();
  timer_el0_set_timeout();

  // unmask timer interrupt
  asm volatile("mov x0, 2             \n");
  asm volatile("ldr x1, =0x40000040   \n");
  asm volatile("str w0, [x1]          \n");

  asm volatile("eret              \n");
}

void test_tasks() {
  proc_init();

  struct task_struct *root_task;
  root_task = task_create(idle);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((uint64_t)root_task));

  task_create(user_startup);
  // task_create(foo);
  // task_create(foo);
  // task_create(foo);
#ifdef CFG_LOG_PROC_SCHED
  _dump_runq();
#endif
  idle();
}