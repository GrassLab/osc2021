#include "proc/task.h"
#include "bool.h"
#include "exec.h"
#include "list.h"
#include "mm.h"
#include "mm/frame.h"
#include "proc.h"
#include "proc/sched.h"
#include "syscall.h"
#include "timer.h"
#include "uart.h"

#include "cfg.h"
#include "log.h"

#ifdef CFG_LOG_PROC_TASK
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

static void foo();

struct task_struct *task_create(void *func, int tid) {
  struct task_struct *t;
  t = (struct task_struct *)kalloc(FRAME_SIZE);
  if (t == NULL) {
    log_println("[task] oops cannot allocate thread");
    return NULL;
  }

  // Normal task is a kernel function, which has already been loaded to memory
  t->entry_point = NULL;
  t->cpu_context.fp = (uint64_t)t + FRAME_SIZE;
  t->cpu_context.lr = (uint64_t)func;
  t->cpu_context.sp = (uint64_t)t + FRAME_SIZE;
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
  struct task_struct *task = get_current();
  uart_println("pid: %d", task->id);
  tf->regs[0] = task->id;
};

// Overwrite current task
// note: int exec(const char *name, char *const argv[]);
void sys_exec(struct trap_frame *tf) {
  struct task_struct *task = get_current();
  const char *name = (const char *)tf->regs[0];
  uart_println("sys exec called");
  // do not get argv in mvp version

  // address of the program code in memory
  void *entry_point = load_program(name);
  task->entry_point = entry_point;

  task->cpu_context.fp = (uint64_t)task + FRAME_SIZE;
  task->cpu_context.lr = (uint64_t)entry_point;
  task->cpu_context.sp = (uint64_t)task + FRAME_SIZE;
  uart_println("thread info replaced");

  // finish process replacement
  // wait for scheduler schedule this thread
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

// note: void exit();
void sys_exit(struct trap_frame *tf) { cur_task_exit(); }

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
  const char *name = "./argv_test.out";
  struct task_struct *task = get_current();
  // address of the program code in memory
  void *entry_point = load_program(name);
  uart_println("program loaded: %x", entry_point);

  task->cpu_context.fp = (uint64_t)task + FRAME_SIZE;
  task->cpu_context.lr = (uint64_t)entry_point;
  task->cpu_context.sp = (uint64_t)task + FRAME_SIZE;

  task->entry_point = entry_point;

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
  root_task = task_create(idle, 0);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((uint64_t)root_task));

  task_create(user_startup, 1);
  // task_create(foo, 1);
  task_create(foo, 2);
  task_create(foo, 3);
#ifdef CFG_LOG_PROC_SCHED
  _dump_runq();
#endif
  idle();
}