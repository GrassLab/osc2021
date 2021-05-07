//# include "exception.h"

# define TASK_MAX_NUM    100
# define TASK_EPOCH      5
# define STACK_SIZE      4096
# define STACK_TOP_IDX   4096-16
# define PRIORITY_MAX    10

/*
struct cpu_context{
  unsigned long long x19;
  unsigned long long x20;
  unsigned long long x21;
  unsigned long long x22;
  unsigned long long x23;
  unsigned long long x24;
  unsigned long long x25;
  unsigned long long x26;
  unsigned long long x27;
  unsigned long long x28;
  unsigned long long fp;  // x29
  unsigned long long lr;  // x30
  unsigned long long sp;
};
*/

enum task_state{
  RUNNING,
  ZOMBIE,
  EXIT,
};

enum task_el{
  USER,
  KERNEL,
};

struct task{
  unsigned long long x19;
  unsigned long long x20;
  unsigned long long x21;
  unsigned long long x22;
  unsigned long long x23;
  unsigned long long x24;
  unsigned long long x25;
  unsigned long long x26;
  unsigned long long x27;
  unsigned long long x28;
  unsigned long long fp;  // x29
  unsigned long long lr;  // x30
  unsigned long long sp;
  int pid;
  enum task_state state;
  int priority;
  int counter;
  int resched_flag;
  void (*invoke_func)();
  enum task_el mode;
  //struct cpu_context context;
  struct task *pre;
  struct task *next;
};

void task_init();
void task_exit();
int task_create(void (*func)(), int priority, enum task_el mode);
int privilege_task_create(void (*func)(), int priority);
int user_task_create(void (*func)(), int priority);
void yield();
void schedule();
int get_pid();
void sys_fork(struct trapframe* trapframe);
void sys_exec(struct trapframe* trapframe);

extern "C" struct task* get_current();
extern "C" void switch_to(struct task *pre, struct task *next);
extern "C" void return_from_fork();
