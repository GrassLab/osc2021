#define MAX_TASK_NR 32

#define TASK_NEW     0
#define TASK_READY   1
#define TASK_RUNNING 2
#define TASK_ZOMBIE  3

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
    unsigned long x29; // fp
    unsigned long sp;
    unsigned long x30; // lr (pc)
};

struct task {
    struct cpu_context ctx;
    int pid;
    int status;
    int counter;
    unsigned int flag;
    int priority;
    char *kernel_stack_page;
    char *ksp;
    char *user_stack_page;
    char *usp;
    int preemptable;
    int resched;
    int free;
    struct trap_frame *tf;
    struct task *next, *prev;
};

struct trap_frame {
    unsigned long regs[31];
    unsigned long sp_el0;
    unsigned long elr_el1; // pc
    unsigned long spsr_el1;
};

void init_ts_pool();
struct task *new_ts();
int add_to_ready(struct task *new);
struct task *thread_create(unsigned long func_addr, unsigned long args);
struct task *pick_next();
int schedule();
int sys_exit();
extern void cpu_switch_to(struct task* prev, struct task* next);
