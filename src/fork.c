
/**
 * The file name fork.c is a little bit confusing, maybe need to fix it
 * Because this file is not actual fork process(No copy original process), see syscall fork()
 * for real fork() 
 */
#include "mm.h"
#include "sched.h"
#include "fork.h"
#include "entry.h"
#include "exception.h"
#include "printf.h"
#include "sched.h"
#include "types.h"

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
        
        // No user stack
        p->stack = (unsigned long) NULL;
    } else {
        // Sysclone and fork, initalize stage for clone and fork user process
        struct pt_regs * cur_regs = task_pt_regs(current);
        *childregs = *cur_regs;
        childregs->regs[0] = 0; // for distinguish it as new child process
        childregs->sp = stack + PAGE_SIZE;
        p->stack = stack;
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
    p->cpu_context.sp = (unsigned long)childregs;
    
    int pid = assignPID();
    task[pid] = p;	
    task[pid]->pid = pid;

    preempt_enable();
    return pid;
}

int move_to_user_mode(unsigned long pc) 
{
    struct pt_regs *regs = task_pt_regs(current);
    memzero((unsigned long)regs, sizeof(* regs));
    regs->pc = pc;
    regs->pstate = PSR_MODE_EL0t;
    unsigned long stack = (unsigned long) kmalloc(PAGE_SIZE);
    if (!stack) { // NULL
        printf("[copy_process] Allocate memory fail");
        return -1;
    }
    
    regs->sp = stack + PAGE_SIZE;
    current->stack = stack;
    return 0;
}

struct pt_regs *task_pt_regs(struct task_struct *tsk) {
    unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
    return (struct pt_regs *)p;
}

long assignPID() 
{
    for (long i = 0;i < NR_TASKS;i++) {
        if (task[i] == NULL) {
            nr_tasks++;
            return i;
        }
    }    

    return -1;
}
