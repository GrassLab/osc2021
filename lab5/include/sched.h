#ifndef __SCHED_H_
#define __SCHED_H_
#include <list.h>
#include "sysreg.h"
#include "current.h"

#define KSTACK_SIZE 0x2000
#define USTACK_SIZE 0x2000
#define TIMESLICE

typedef enum {
    TASK_RUNNING,
    TASK_PAUSED,
    TASK_STOPPED
} state_t;

typedef unsigned pid_t;
typedef unsigned long time_slice_t;

struct cpu_context {
	unsigned long x19;
	unsigned long x20;
	unsigned long x21;
	unsigned long x22;
	unsigned long x23;
	unsigned long x24;
	unsigned long x25;
	unsigned long x26;
	unsigned long x27;
	unsigned long x28;
	unsigned long fp;
	unsigned long sp;
	unsigned long pc;
};

struct task_struct {
    /* cpu_context need to be the first element of task_struct */
    struct cpu_context cpu_context;

    char *stack;
    char *kstack;
    char *user_prog;
    pid_t pid;
    int exitcode;
    unsigned need_sched;
    unsigned long timer;
    unsigned long last_tick;
    unsigned long remained_tick;
    state_t state;
    struct list_head list;
};

extern size_t GLB_PID;

void set_init_thread();
void schedule();
void task_pause(struct task_struct *);
void schedule_kthread(void *cb);
struct task_struct *alloc_user_task(void *prog, const char *argv[]);
void add_task(struct task_struct *);
void del_task(struct task_struct *);
void kill_task(struct task_struct *target, int status);

#endif