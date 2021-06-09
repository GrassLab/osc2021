#include "sched.h"
#include "entry.h"
#include "printf.h"
#include "mm.h"
#include "utils.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct *task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

struct list_head runqueue[MAX_PRIO];

void preempt_disable(void)
{
    current->preempt_count++;
}

void preempt_enable(void)
{
    current->preempt_count--;
}

void _schedule(void)
{
    preempt_disable();
    
    int next, c;
    struct task_struct *p;
    while (1) {
        c = -1;
        next = 0;
        for (int i = 0;i < NR_TASKS;i++) {
            p = task[i];
            if (p && p->state == TASK_RUNNING && p->counter > c) {
                c = p->counter;
                next = i;
            }
        }
        if (c) {
            break;
        }
        for (int i = 0;i < NR_TASKS;i++) {
            p = task[i];
            if (p) {
                p->counter = (p->counter >> 1) + p->priority;
            }
        }
    }
    
    //printf("[_schedule] next scheduled - pid = %d\n", next);
    switch_to(task[next], next);

    preempt_enable();
}

void schedule(void) 
{
    current->counter = 0;
    _schedule();
}

void switch_to(struct task_struct *next, int index) 
{
    if (current == next)
        return;

    //printf("[switch_to] Tasks state before cotext switch:\n");
    //dumpTasksState();
    printf("[switch_to] context switch! next scheduled - pid = %d\n", index);

    struct task_struct *prev = current;
    current = next;
    set_pgd(next->mm.pgd); /* Switch page table for user space */
    cpu_switch_to(prev, next);
}


void schedule_tail(void)
{
    preempt_enable();
}

void timer_tick()
{
    --current->counter;
}

void task_preemption()
{
    if (current->counter>0 || current->preempt_count >0) {
        return;
    }
    current->counter=0;

    enable_irq();   // Enter by interrupt Because default irq is off
    _schedule();
    disable_irq();
}

void exit_process(void) {
    printf("[exit_process] Process exit, pid = %d\n", current->pid);
    preempt_disable();
    current->state = TASK_ZOMBIE;

    // free all alloacted user page
    for (int i = 0;i < current->mm.user_pages_count;i++) {
        #ifdef __DEBUG_MM
        printf("[exit_process] Free page: current->mm.user_pages[%d].phys_addr = 0x%x\n", i, current->mm.user_pages[i].phys_addr);
        #endif
        free_page((void *)current->mm.user_pages[i].phys_addr);
    }
    #ifdef __DEBUG_MM
    dump_buddy(); // dump boddy system
    #endif

    preempt_enable();
    schedule();
}

void kill_zombies(void) {
    // reclaim threads marked as DEAD
    for (int i = 0;i < NR_TASKS;i++) {
        if (task[i]->state == TASK_ZOMBIE) {
            // TODO:
            // free all allocated kernel page
            // kfree(task[i]);
            // task[i] = NULL;
        }
    }
}

void dumpTasksState() {
    printf("=========Tasks state=========\n");
    for (int i = 0;i < nr_tasks;i++) {
        printf("Task %d: ", i);

        switch(task[i]->state) {
        case TASK_RUNNING:
            printf("RUNNING");
            break;
        case TASK_WAITING:
            printf("WAITING");
            break;
        case TASK_ZOMBIE: 
            printf("ZOMBIE");
            break;
        default:
            printf("Unknown State");
        }
        printf("\n");
    } 
}