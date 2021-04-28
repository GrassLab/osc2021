#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define NULL ((void*)0)
#define MAX_THREAD_COUNT 64
#define THREAD_SIZE (1<<12)

#define RUNNING 0
#define READY   1 // waiting in running queue
#define DEAD    2
#define ZOMBIE  3
#define WAITING 4 // wait in waiting queue

#define PF_KTHREAD  0x2	

#ifndef __ASSEMBLER__

#define INIT_THREAD { \
    {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
    0, 0, 1, 1, 0 \
}

typedef struct _cpu_context{
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
    unsigned long fp; // x29
    unsigned long pc; // x30
    unsigned long sp;
    
} cpu_context;

typedef struct _thread{
    cpu_context cpu_context;
    unsigned int id;
    unsigned int state;
    unsigned int priority;
    unsigned int counter;
    // if preempt_flag > 0, the thread must not be interrupted
    unsigned int preempt_flag;
    unsigned long stack;
	unsigned long flags;
} thread;

typedef struct _pt_regs {
	unsigned long regs[31];
	unsigned long sp;
	unsigned long pc;
	unsigned long pstate;
} pt_regs;

extern int thread_count;
extern thread* run_queue[MAX_THREAD_COUNT];
extern thread* current;
//extern void cpu_switch_to(thread* prev, thread* next);
void swithch_to(thread* next);
extern int create_thread(unsigned long clone_flags, unsigned long func, unsigned long arg, unsigned long stack);
extern void scheduler(void);
extern void idle(void);
extern void delay(unsigned long count);
void _exec(char *name, char **argv);

#endif
#endif