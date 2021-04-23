#include "bool.h"
#include "mm.h"
#include "mm/frame.h"
#include "sched.h"
#include "uart.h"

struct task_struct *task_create(void *func, int tid);
void exit();
void foo();
void task_schedule();

struct task_struct *tasks[4];

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
  t->status = TASK_STATUS_LIVE;
  t->id = tid;
  tasks[tid] = t;

  uart_println("task created: id:%d struct:%x func:%x", t->id, t,
               t->cpu_context.lr);
  return t;
}

void task_schedule() {
  uart_println("Scheduler called");
  struct task_struct *cur = get_current();
  struct task_struct *next = cur;
  int _cur_id = cur->id;

  for (;;) {
    if (_cur_id < 3) {
      next = tasks[_cur_id + 1];
    } else {
      next = tasks[1];
    }
    _cur_id = next->id;
    if (next == cur) {
      break;
    }
    if (next->status == TASK_STATUS_LIVE) {
      break;
    }
  }

  if (next->status != TASK_STATUS_LIVE) {
    uart_println("no task alived");
    while (1) {
      ;
    }
  }
  // uart_println("cur:%d next:%d", cur->id, next->id);
  switch_to(cur, next);
}

void exit() {
  // Exit current running thread;
  struct task_struct *task = get_current();
  task->status = TASK_STATUS_DEAD;
  uart_println("task %d called exit", task->id);
  task_schedule();
}

void foo() {
  struct task_struct *task = get_current();
  for (int i = 0; i < 2; ++i) {
    uart_println("Thread id: %d -> loop:%d", task->id, i);
    task_schedule();
  }
  exit();
}

void test_tasks() {
  struct task_struct *root_task;
  root_task = task_create(foo, 0);
  uart_println("t = %x", root_task);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((unsigned long)root_task));

  task_create(foo, 1);
  task_create(foo, 2);
  task_create(foo, 3);
  task_schedule();
}