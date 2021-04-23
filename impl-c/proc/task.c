#include "proc/task.h"
#include "proc.h"
#include "proc/sched.h"

#include "bool.h"
#include "list.h"
#include "mm.h"
#include "mm/frame.h"
#include "uart.h"

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
    uart_println("oops cannot allocate thread");
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
  uart_println("entry created: %x %x", entry->task, entry->list);

  uart_println("task created: id:%d struct:%x func:%x", t->id, t,
               t->cpu_context.lr);
  return t;
}

void cur_task_exit() {
  // Exit current running thread;
  struct task_struct *task = get_current();
  task->status = TASK_STATUS_DEAD;
  uart_println("task %d called cur_task_exit", task->id);
  task_schedule();
}

void test_tasks() {
  scheduler_init();

  struct task_struct *root_task;
  root_task = task_create(idle, 0);
  uart_println("t = %x", root_task);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((unsigned long)root_task));

  task_create(foo, 1);
  task_create(foo, 2);
  task_create(foo, 3);
  _dump_runq();
  idle();
}