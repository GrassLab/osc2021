#ifndef __SCHEDULER__
#define __SCHEDULER__

#define RUN_STATE       0
#define ZOMBIE_STATE    1

#define CODE_SEG        0   // text, data, and bss
#define USTACK_SEG      1
#define KSTACK_SEG      2

#define TASK_LIFE_TIME  5

typedef struct cpu_context_struct {
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
    unsigned long lr;
    unsigned long spsr_el1;
    unsigned long elr_el1;
    unsigned long sp_el0;
    unsigned long sp;
} cpu_context_struct;

typedef struct mm_struct {
    unsigned long size;
    unsigned long address;
    int seg_type;   //segmentment type (stack or code)
    int owner_num;
    struct mm_struct *next;
    struct mm_struct *prev;
    struct mm_struct *same_mm_owner_next;   // circle list
    struct mm_struct *same_mm_owner_prev;   // circle list
} mm_struct;

typedef struct task_struct {
    cpu_context_struct cpu_context;
    unsigned int pid;
    mm_struct *mm;
    int state;
    unsigned long last_time;
    unsigned long use_time;
    struct task_struct *next;      // in queue's next
    struct task_struct *prev;      // in queue's prev
} task_struct;

static task_struct *get_current(void) {
    task_struct *task;
    asm volatile("mrs %[task], tpidr_el1\n\t":[task] "=r" (task)::);
    return task;
}

#define current get_current()

void push_terminal_queue(task_struct *task);
void push_run_queue(task_struct *task);
void pop_run_queue_by_pid(int pid);
task_struct *pop_terminal_queue_head();
task_struct *set_current();
void schedule();

#endif