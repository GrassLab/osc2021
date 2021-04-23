#include "bool.h"
#include "list.h"
#include "mm.h"
#include "mm/frame.h"
#include "sched.h"
#include "uart.h"

struct list_head run_queue;
struct list_head exited;

struct task_entry {
  struct list_head list;
  struct task_struct *task;
};

struct task_struct *task_create(void *func, int tid);
void exit();
void foo();
void task_schedule();

void dump_runq() {
  struct list_head *list_head = &run_queue;
  struct list_head *entry;
  uart_printf("Runqueue");
  int i = 0;
  for (entry = list_head->next; entry != list_head; entry = entry->next) {
    struct task_struct *task = ((struct task_entry *)entry)->task;
    uart_printf("->[%d, %x]", task->id, task);
    i++;
  }
  uart_println("");
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

void task_schedule() {
  uart_println("Scheduler called");
  struct task_struct *cur = get_current();
  struct task_struct *next;

  struct task_entry *entry_next;
  if (!list_empty(&run_queue)) {
    while (1) {
      entry_next = (struct task_entry *)list_pop_front(&run_queue);
      next = entry_next->task;
      if (next->status == TASK_STATUS_ALIVE) {
        list_push(&entry_next->list, &run_queue);
        break;
      } else {
        list_push(&entry_next->list, &exited);
      }
    }
    uart_println("cur:%d next:%d", cur->id, next->id);
    switch_to(cur, next);
  }
}

void kill_zombies() {
  struct list_head *entry;
  struct task_struct *task;
  while (!list_empty(&exited)) {
    entry = list_pop(&exited);
    task = ((struct task_entry *)entry)->task;
    uart_println("recycle space for task:%d", task->id);
    list_del(entry);
    kfree(task);
    kfree(entry);
  }
}

void exit() {
  // Exit current running thread;
  struct task_struct *task = get_current();
  task->status = TASK_STATUS_DEAD;
  uart_println("task %d called exit", task->id);
  task_schedule();
}

void idle() {
  while (1) {
    uart_println("idle scheduling...");
    _wait();
    task_schedule();
    kill_zombies();
  }
}

void foo() {
  struct task_struct *task = get_current();
  for (int i = 0; i < 2; ++i) {
    uart_println("Thread id: %d -> loop:%d", task->id, i);
    _wait();
    task_schedule();
  }
  exit();
}

void scheduler_init() {
  list_init(&run_queue);
  list_init(&exited);
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
  dump_runq();
  idle();
}