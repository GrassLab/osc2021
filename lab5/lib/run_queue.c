#include <sched.h>
#include <types.h>
#include <dynamic.h>
#include <printf.h>

void run_queue_push(struct task_struct* t, struct run_queue_struct* q) {
  
  if(q->tail == null) {
    q->head = t;
    q->tail = t;
  }
  else {
    q->tail->next = t;
    q->tail = t;
  }

}

struct task_struct* run_queue_pop(struct run_queue_struct* q) {
  struct task_struct* t;
  
  if(q->head == null)
    return null;
  
  if(q->head == q->tail) 
    q->tail = q->tail->next;

  t = q->head;
  q->head = q->head->next;
  
  return t;
}

void run_queue_status(struct task_struct* q) {
  struct task_struct *t;
  t = q;  
  if(t == null)
    return;

  while(t != null) {
    printf("task_id: %d -->", t->task_id);
    t = t->next;
  }
  printf("null\n");

}