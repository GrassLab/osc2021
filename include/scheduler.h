#include "vfs.h"
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#ifndef __ASSEMBLER__

#define NULL ((void*)0)
#define NR_TASKS 64
#define THREAD_SIZE (1<<12)

#define RUNNING 0
#define READY   1 // waiting in running queue
#define DEAD    2
#define ZOMBIE  3
#define WAITING 4 // wait in waiting queue

#define PF_KTHREAD  0x2	
#define FD_MAX_SIZE 5

extern struct task_struct *current;
extern struct task_struct * task[NR_TASKS];
extern int nr_tasks;

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

#define MAX_PROCESS_PAGES 16

struct user_page {
	unsigned long phys_addr;
	unsigned long virt_addr;
};

struct mm_struct {
	unsigned long pgd;
	int user_pages_count;
	struct user_page user_pages[MAX_PROCESS_PAGES];
	int kernel_pages_count;
	unsigned long kernel_pages[MAX_PROCESS_PAGES];
};

struct task_struct {
	struct cpu_context cpu_context;
    int id;
	long state;	
	long counter;
	long priority;
	long preempt_count;
	unsigned long flags;
	struct mm_struct mm;
    file *fd_table[FD_MAX_SIZE];
    int child;
};

struct pt_regs *task_pt_regs(struct task_struct *tsk);

struct pt_regs {
	unsigned long regs[31];
	unsigned long sp;
	unsigned long pc;
	unsigned long pstate;
};

void swithch_to(struct task_struct* next);
extern void scheduler(void);
extern void idle(void);
extern void delay(unsigned long count);
void _exec(char *name, char **argv);
void timer_tick();
void _exit();
void ret_from_fork();
void ret_to_user();

#define INIT_TASK \
/*cpu_context*/ 	{ { 0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	 		0, 0, 0, 2, 0, PF_KTHREAD, \
/* mm */ 				{ 0, 0, {{0}}, 0, {0}}, \
/* fd_table */ 			0, \
/* child */				0 \
					}	

#endif
#endif