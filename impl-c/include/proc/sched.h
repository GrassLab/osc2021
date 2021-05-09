#pragma once

#include "list.h"

extern struct list_head run_queue;
extern struct list_head exited;

struct task_entry {
  struct list_head list;
  struct task_struct *task;
};

void scheduler_init();

// Root of all tasks, would recycle exited thread
void idle();

void task_schedule();

void _dump_runq();
