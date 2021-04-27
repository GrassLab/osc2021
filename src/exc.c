#include "exc.h"

#include <stddef.h>

#include "mem.h"

unsigned long interrupt_status = 1;

#define TASK_WAIT 0
#define TASK_RUN 1

typedef struct task_pending {
  unsigned long priority;
  unsigned long status;
  void *task;
  void *data;
  struct task_pending *next;
} task_pending;

task_pending *task_list = NULL;

void enable_interrupt() {
  if (interrupt_status > 0) {
    interrupt_status--;
    if (interrupt_status == 0) {
      asm volatile("msr DAIFClr, 0xf;");
    }
  }
}

void disable_interrupt() {
  if (interrupt_status == 0) {
    asm volatile("msr DAIFSet, 0xf;");
  }
  interrupt_status++;
}


void exec_usr(void *addr, void *sp, unsigned long pstate) {
  disable_interrupt();
  unsigned long stat = interrupt_status;
  _exec_usr(addr, sp, pstate);
  interrupt_status = stat;
  enable_interrupt();
}

void exec_task() {
  while (task_list != NULL && task_list->status == TASK_WAIT) {
    task_list->status = TASK_RUN;
    task_pending *tp = task_list;
    enable_interrupt();
    ((void (*)(void *))(tp->task))(tp->data);
    disable_interrupt();
    task_list = task_list->next;
    kfree(tp);
  }
}

void add_task(void *task, void *data, unsigned long priority) {
  task_pending *new_task = kmalloc(sizeof(task_pending));
  new_task->data = data;
  new_task->priority = priority;
  new_task->task = task;
  new_task->status = TASK_WAIT;

  if (task_list == NULL || task_list->priority > priority) {
    // preempt
    new_task->next = task_list;
    task_list = new_task;
    exec_task();
  } else {
    // add queue
    task_pending *tp_itr = task_list;
    while (tp_itr->next != NULL && tp_itr->next->priority <= priority) {
      tp_itr = tp_itr->next;
    }
    new_task->next = tp_itr->next;
    tp_itr->next = new_task;
  }
}

unsigned long get_int_stat() {
  return interrupt_status;
}

void set_int_stat(unsigned long stat) {
  interrupt_status = stat;
}