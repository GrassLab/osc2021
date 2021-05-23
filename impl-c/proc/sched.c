#include "proc/sched.h"
#include "proc/task.h"

#include "list.h"
#include "mm.h"
#include "uart.h"

#include "cfg.h"
#include "log.h"

#ifdef CFG_LOG_PROC_SCHED
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

// proc/sched.S
void switch_to(struct task_struct *prev, struct task_struct *next);

static void kill_zombies();

struct list_head run_queue;
struct list_head exited;

void proc_init() {
  new_tid = 0;
  list_init(&run_queue);
  list_init(&exited);
}

void task_schedule() {
  struct task_struct *cur = get_current();
  struct task_struct *next;

  struct task_entry *entry_next;
#ifdef CFG_LOG_PROC_SCHED
  _dump_runq();
#endif
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
    log_println("[schedule] switch thread %d->%d", cur->id, next->id);
    switch_to(cur, next);
  }
}

void idle() {
  while (1) {
    uart_println("idle scheduling...");
    _wait();
    task_schedule();
    kill_zombies();
  }
}

void kill_zombies() {
  struct list_head *entry;
  struct task_struct *task;
  while (!list_empty(&exited)) {
    entry = list_pop(&exited);
    task = ((struct task_entry *)entry)->task;
    log_println("recycle space for task:%d", task->id);
    list_del(entry);

    kfree(entry);
    // TODO: manage parent code free and child code free
    // if (task->code) {
    //   kfree(task->code);
    // }
    if (task->user_stack) {
      kfree((void *)task->user_stack);
    }
    kfree((void *)task->kernel_stack);
    kfree(task);
  }
}

void _dump_runq() {
  struct list_head *list_head = &run_queue;
  struct list_head *entry;
  uart_printf("%s%s", LOG_DIM_START, "[schedule] Runqueue");
  int i = 0;
  for (entry = list_head->next; entry != list_head; entry = entry->next) {
    struct task_struct *task = ((struct task_entry *)entry)->task;
    uart_printf("->[%d, %x]", task->id, task);
    i++;
  }
  uart_println("%s", LOG_DIM_END);
}