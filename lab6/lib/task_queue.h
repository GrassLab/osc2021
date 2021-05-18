#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <sched.h>

struct task_queue_struct {
  struct task_struct* head;
  struct task_struct* tail;
};

void task_queue_push(struct task_struct* t, struct task_queue_struct* q);
struct task_struct* task_queue_pop(struct task_queue_struct* q);
void task_queue_status(struct task_queue_struct* q);
void task_queue_remove(struct task_struct* t, struct task_queue_struct* q);
#endif