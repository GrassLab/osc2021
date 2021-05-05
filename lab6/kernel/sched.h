#ifndef _SCHED_H_
#define _SCHED_H_
#include <types.h>
#include <task.h>
#include <task_queue.h>

#define TASK_POOL_SIZE 0x40
#define TASK_STACK_SIZE 0x1000
#define KERNEL_STACK_ADDR 0x11000000
#define USER_STACK_ADDR (0x11000000 + TASK_STACK_SIZE * TASK_POOL_SIZE)

#define TASK_STATUS_DEAD 0
#define TASK_STATUS_READY 1


struct trapframe {
  size_t x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  size_t x10, x11, x12, x13, x14, x15, x16, x17, x18, x19;
  size_t x20, x21, x22, x23, x24, x25, x26, x27, x28, x29;
  size_t x30, sp_el0, elr_el1, spsr_el1; 
};

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
  int exit_status;
  int resched;
  void* start;
  size_t size;
  void* stack;
  struct task_struct* next;
};

struct task_struct task_pool[TASK_POOL_SIZE];
struct task_queue_struct run_queue;

extern struct task_struct* get_current();
extern void switch_to(struct task_struct* prev, struct task_struct* next);

struct task_struct* privilege_task_create( void(*func)() );
void schedule();
void task_init();
void idle_task();

extern void disable_interrupt();
extern void enable_interrupt();
extern void* not_syscall();
extern void* kernel_exit();
struct trapframe* get_trapframe(struct task_struct* t);

void sys_exit(int status);
void do_exit(int status);
int sys_fork();
int do_fork();
int sys_exec(const char* name, char* const argv[]);
int do_exec(const char* name, char* const argv[]);
int sys_getpid();
int do_getpid();

void* load_program(const char* name);
void* fork_memcpy();
void* exec_set_argv(void* stack, int argc, char* const argv[]);
extern int exit();

void task_test1_init();
void task_test2_init();
#endif