#ifndef _SCHED_H
#define _SCHED_H

#define THREAD_CPU_CONTEXT			0 		// offset of cpu_context in task_struct 

#ifndef __ASSEMBLER__

#define THREAD_SIZE				4096

#define NR_TASKS				64 

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]

#define TASK_RUNNING				0
#define TASK_ZOMBIE				1
#define TASK_FREE				2
#define PF_KTHREAD				0x00000002	

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

#define MAX_PROCESS_PAGES			16	
#define MAX_FILE_NUM 				10

struct block {
    struct block *next;
    int order;         // buddy system order
    int page_nums;     // how many pages in this block
    int pfn;
    int record_index;
};

// buddy system order
struct free_area {
    struct block *head;
    struct block *tail;
    unsigned long nr_free;
};

struct mm_struct {
	unsigned long pgd;
	int user_pages_count;
	struct block *user_pages[MAX_PROCESS_PAGES];
	int kernel_pages_count;
	struct block *kernel_pages[MAX_PROCESS_PAGES];
	unsigned long user_va[MAX_PROCESS_PAGES];
};

struct task_struct {
	struct cpu_context cpu_context;
	long state;	
	long counter;
	long priority;
	long preempt_count;
	unsigned long flags;
	struct file *file_desp[MAX_FILE_NUM];
	struct mm_struct mm;
	int  task_id;
};

extern void sched_init(void);
extern void schedule(void);
extern void timer_tick(void);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void switch_to(struct task_struct* next);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
extern void exit_process(void);

#define INIT_TASK \
/*cpu_context*/ { { 0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	 0,0,1, 0, PF_KTHREAD, \
/* mm */ { 0, 0, {{0}}, 0, {0}}, 0 \
}
#endif
#endif
