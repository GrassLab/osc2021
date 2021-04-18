#ifndef _SCHED_H_
#define _SCHED_H_
#include <types.h>
#include <task.h>
#include <task_queue.h>

#define TASK_POOL_SIZE 0x40
#define TASK_STACK_SIZE 0x1000
#define KERNEL_STACK_ADDR 0x10000000
#define USER_STACK_ADDR (0x10000000 + TASK_STACK_SIZE * TASK_POOL_SIZE)

#define TASK_STATUS_DEAD 1
#define TASK_STATUS_LIVE 2

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
  void* stack;
  int status;
  int exit_status;
  struct task_struct* next;
};

struct task_struct task_pool[TASK_POOL_SIZE];
struct task_queue_struct run_queue;

extern struct task_struct* get_current();
extern void switch_to(struct task_struct* prev, struct task_struct* next);

struct task_struct* privilege_task_create( void(*func)() );
void task_init();
void idle_task();
void schedule();

extern void* disable_interrupt();
extern void* enable_interrupt();
void sys_exit(int status);
void do_exit(int status);
int sys_fork();
int do_fork();
int sys_exec(void(*func)());
int do_exec(void(*func)());

#endif