#include "scheduler.h"
#include "allocator.h"
#include "utils.h"
#include "uart.h"
#include "cpio.h"
#include "vfs.h"

#define CREATE_SUCCESS 0
#define CREATE_FAIL    1

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

static void preempt_disable(void) {
    if(current) current->preempt_count++;
}

static void preempt_enable(void) {
	if(current) current->preempt_count--;
}

void delay(unsigned long count) {
    while(count--){};
}

struct pt_regs * task_pt_regs(struct task_struct *tsk)
{
	unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
	return (struct pt_regs *)p;
}

int create_thread(unsigned long clone_flags, unsigned long fn, unsigned long arg)
{
	preempt_disable();
	struct task_struct *p;

	unsigned long page = kmalloc(1<<12);
	p = (struct task_struct *) page;
	struct pt_regs *childregs = task_pt_regs(p);

	if (!p)
		return -1;

	if (clone_flags & PF_KTHREAD) {
		p->cpu_context.x19 = fn;
		p->cpu_context.x20 = arg;
	} else {
		struct pt_regs * cur_regs = task_pt_regs(current);
		*childregs = *cur_regs;
		childregs->regs[0] = 0;
		copy_virt_memory(p);
	}
	p->flags = clone_flags;
	p->priority = current->priority;
	p->state = RUNNING;
	p->counter = p->priority;
	p->preempt_count = 1; //disable preemtion until schedule_tail

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs;
	int pid = nr_tasks++;
    p->id = pid;
	task[pid] = p;	    

	preempt_enable();
	return pid;
}



void _schedule(void)
{
	preempt_disable();
	int next,c;
	struct task_struct * p;
	while (1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			if (p && p->state == RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	switch_to(task[next]);
	preempt_enable();
}

void scheduler(void)
{
	current->counter = 0;
	_schedule();
}

void switch_to(struct task_struct * next) 
{
	if (current == next)
		return;
	struct task_struct * prev = current;
	current = next;
	set_pgd(next->mm.pgd);
	cpu_switch_to(prev, next);
}

void schedule_tail(void) {
	preempt_enable();
}

void timer_tick() {
    --current->counter;
    if(current->counter > 0 || current->preempt_count >0) return;
    current->counter = 0;
    uart_puts("timer tick\n");
    scheduler();
}

void _exit() {
    preempt_disable();
    for (int i = 0; i < NR_TASKS; i++){
		if (task[i] == current) {
			task[i]->state = ZOMBIE;
			break;
		}
	}
    uart_printf("thread %d is zombie\n", current->id);
    preempt_enable();
    scheduler();
}

void idle() {
    // kill zombies
    while(1) {
        preempt_disable();
        for(int i = 0; i < NR_TASKS; i++) {
            struct task_struct *p = task[i];
            if(p && p->state == ZOMBIE) {
                for(int i = 0; i < FD_MAX_SIZE; i++) {
                    if(p->fd_table[i]) {
                        vfs_close(p->fd_table[i]);
                    }
                }
                free_page((unsigned long)p, THREAD_SIZE);
                nr_tasks--;
            }
        }
        
        preempt_enable();
        uart_printf("I am zombie killer, remain %d threads\n", nr_tasks);
        delay(1000000);
        scheduler();
    }
}

void _exec(char *name, char **argv) {
    unsigned long ret = load_user_program_withArgv(name, argv);
    if(!ret) {
        uart_puts("load user program fail...\n");
        _exit();
        return;
    }
    else {
        uart_puts("load user program success...\n");
    }
}