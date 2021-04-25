#include "task_queue.h"
#include <sched.h>
#include <types.h>
#include <dynamic.h>
#include <printf.h>

void task_queue_push(struct task_struct* t, struct task_queue_struct* q) {
  
  if(q->tail == null) {
    q->head = t;
    q->tail = t;
  }
  else {
    q->tail->next = t;
    q->tail = t;
  }

  q->tail->next = null;
}

struct task_struct* task_queue_pop(struct task_queue_struct* q) {
  struct task_struct* t;
  
  if(q->head == null)
    return null;
  
  if(q->head == q->tail) 
    q->tail = q->tail->next;

  t = q->head;
  q->head = q->head->next;
  
  t->next = null;

  return t;
}

void task_queue_remove(struct task_struct* t, struct task_queue_struct* q) {
  struct task_struct* prev_t, *now_t;
  now_t = q->head;
  prev_t = null;
  //traversal, not good, should use double linked list
  while(now_t != null) {

    if(now_t == t) { 
      if(prev_t != null) {
        
        prev_t->next = now_t->next;
        
        if(now_t == q->tail) {
          q->tail = prev_t;
        }

        return;
      }
      else {

        q->head = null;
        q->tail = null;
        
        return;
      }
    }

    prev_t = now_t;
    now_t = now_t->next;
  }
  
}

void task_queue_status(struct task_queue_struct* q) {
  struct task_struct *t;
  t = q->head;  
  if(t == null)
    return;

  while(t != null) {
    printf("task_id: %d -->", t->task_id);
    t = t->next;
  }
  printf("null\n");
  
  printf("head: task_id: %d\n", q->head->task_id);

  printf("tail: task_id: %d\n", q->tail->task_id);
}