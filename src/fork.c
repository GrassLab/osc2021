#include "mm.h"
#include "sched.h"
#include "fork.h"
#include "entry.h"
#include "exception.h"
#include "printf.h"

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack) {
    preempt_disable();
    struct task_struct *p;

    p = (struct task_struct *) kmalloc(PAGE_SIZE);
    if (!p) { // NULL
        printf("[copy_process] Allocate memory fail");
        return -1;
    }

    struct pt_regs *childregs = task_pt_regs(p);
    memzero((unsigned long)childregs, sizeof(struct pt_regs));
    memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));
    
    if (clone_flags & PF_KTHREAD) {
        // Kernel thread
        p->cpu_context.x19 = fn;
        p->cpu_context.x20 = arg;
    } else {
        // TODO:
        //    Clone user process
    }
    
    p->flags = clone_flags;
    p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
    // disable preemption until schedule_tail, 
    // meaning that after the task is executed it should not be rescheduled
    // until it completes some initialization work.
    p->preempt_count = 1; 
    
	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

    int pid = nr_tasks++;
	task[pid] = p;	
    task[pid]->pid = pid;

    preempt_enable();
    return pid;
}

struct pt_regs *task_pt_regs(struct task_struct *tsk) {
    unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
    return (struct pt_regs *)p;
}