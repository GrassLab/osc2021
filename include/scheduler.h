#include "vfs.h"
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
#define FD_MAX_SIZE 5

#ifndef __ASSEMBLER__

#define INIT_THREAD { \
    {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
    0, 0, 1, 1, 0 \
}

typedef struct _cpu_context{
    // kernel
    unsigned long x19; // 0th
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
    unsigned long lr; // x30
    unsigned long sp;

    unsigned long spsr; // 13th
    unsigned long elr;

    // user
    unsigned long usp;

    unsigned long ux0;
    unsigned long ux1;
    unsigned long ux2;
    unsigned long ux3;
    unsigned long ux4;
    unsigned long ux5;
    unsigned long ux6;
    unsigned long ux7;
    unsigned long ux8;
    unsigned long ux9;
    unsigned long ux10;
    unsigned long ux11;
    unsigned long ux12;
    unsigned long ux13;
    unsigned long ux14;
    unsigned long ux15;
    unsigned long ux16;
    unsigned long ux17;
    unsigned long ux18;
    unsigned long ux19;
    unsigned long ux20;
    unsigned long ux21;
    unsigned long ux22;
    unsigned long ux23;
    unsigned long ux24;
    unsigned long ux25;
    unsigned long ux26;
    unsigned long ux27;
    unsigned long ux28;
    unsigned long ufp;
    unsigned long ulr;

} cpu_context;

typedef struct _thread{
    cpu_context cpu_context;
    unsigned int id;
    unsigned int state;
    unsigned int priority;
    unsigned int counter;
    // if preempt_flag > 0, the thread must not be interrupted
    unsigned int preempt_flag;
    file *fd_table[FD_MAX_SIZE];
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
void timer_tick();
void _exit();

#endif
#endif