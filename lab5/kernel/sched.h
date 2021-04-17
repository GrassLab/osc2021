#ifndef _SCHED_H_
#define _SCHED_H_
#include <types.h>

#define TASK_POOL_SIZE 0x40
#define TASK_STACK_SIZE 0x1000
#define KERNEL_STACK_ADDR 0x10000000
#define USER_STACK_ADDR (0x10000000 + TASK_STACK_SIZE * TASK_POOL_SIZE)

#define TASK_STATUS_DEAD 0
#define TASK_STATUS_LIVE 1

struct context {
  size_t x19;
  size_t x20;
  size_t x21;
  size_t x22;
  size_t x23;
  size_t x24;
  size_t x25;
  size_t x26;
  size_t x27;
  size_t x28;
  size_t fp; //frame pointer, stack base address
  size_t lr; //return address
  size_t sp; //stack pointer
  };

struct task_struct {
  struct context ctx;
  size_t task_id;
  void* kstack;
  int status;
  struct task_struct* next;
};


struct run_queue_struct {
  struct task_struct* head;
  struct task_struct* tail;
};

struct task_struct task_pool[TASK_POOL_SIZE];
struct run_queue_struct run_queue;

struct task_struct* privilege_task_create( void(*func)() );
extern struct task_struct* get_current();
extern void switch_to(struct task_struct* prev, struct task_struct* next);
void schedule();
void task_init();
void idle_task();

void run_queue_push(struct task_struct* t, struct run_queue_struct* q);
struct task_struct* run_queue_pop(struct run_queue_struct* q);
void run_queue_status(struct task_struct* q);
#endif