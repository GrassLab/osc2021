#ifndef _SCHED_H
#define _SCHED_H

#define THREAD_CPU_CONTEXT 0  // offset of cpu_context in task_struct

#ifndef __ASSEMBLER__

#include "irq.h"


#define PF_KTHREAD 0x00000002

#define THREAD_SIZE 4096

// Max number of tasks.
#define NR_TASKS 64

#define FIRST_TASK (task[0])
#define LAST_TASK (task[NR_TASKS - 1])

#define TASK_RUNNING 0
#define TASK_ZOMBIE 1


extern struct task_struct *current;
extern struct task_struct *task[NR_TASKS];
extern int nr_tasks;


typedef struct task_struct {
    // Holds the task state (TASK_RUNNING/etc).
    long state;
    // This is the time slice to give the task every time it's scheduled (after
    // the time slice reaches 0). This helps determine the priority of the task
    // because the more priority, the larger its time slice.
    long priority;
    // Indicates if the current task is preemptable or not. It may not be
    // preemptable if the task is currently running in the scheduler, for
    // example. Note that it is a counter because preempt_disable could be
    // called multiple times.
    long preempt;
    // Custom field added by me
    int pid;
	struct task_struck *next;
}task_struct;


extern void set_pgd(unsigned long);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void schedule_tail(void);
extern void timer_tick();
extern void cpu_switch_to(struct task_struct *, struct task_struct *);
extern void schedule(void);
extern void exit_process();




void add_task();
void setpriority(char* args);
void preempt(char* args);
void getpid();


#define INIT_TASK      { /* state etc */ 0, 0, 0, 0, NULL }


#endif

#endif
