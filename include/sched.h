#ifndef _SCHED_H
#define _SCHED_H


#define THREAD_CPU_CONTEXT          0       // offset of cpu_context in task_struct 

#ifndef __ASSEMBLER__ 

#include "list.h"

#define THREAD_SIZE                 4096

#define NR_TASKS                    64     // The max number of task

#define TASK_RUNNING                0
#define TASK_WAITING				1
#define TASK_ZOMBIE                 2

#define PF_KTHREAD                  0x00000002	

#define MAX_PRIO                    140     // Max proirity number of runqueue (140 is same as linux kernel)

#define KERNEL_SHELL_PID            0

extern struct task_struct *current;         // current executing task
extern struct task_struct *task[NR_TASKS]; 
extern int nr_tasks;

// Each runqueue have corresponding prority(array number)
// The lower number is more significant than higher one
// TODO:
extern struct list_head runqueue[MAX_PRIO];

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

/**
 * @stack: User stack
 * @flags: Some flags such as Kernel/User thread 
 */
struct task_struct {
    struct cpu_context cpu_context;
    long state;	
    long counter;
    long priority;
    long preempt_count;
    unsigned long stack; 
    unsigned long flags;
    long pid;
    struct list_head run_list;
};

extern void schedule(void);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void timer_tick(void);
extern void switch_to(struct task_struct * next, int index);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
extern void task_preemption();
extern void exit_process(void);
extern void kill_zombies(void);

extern void dumpTasksState(void);

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0, 0, 1, 0, 0, PF_KTHREAD, \
/* task id */	0 \
}

#endif /* __ASSEMBLER__ */
#endif /* _SCHED_H */