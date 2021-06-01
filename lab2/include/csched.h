#define MAX_TASK_NR 32
#define MAX_VMA_NR 128
#define MAX_WAIT_NR 8
#define MAX_WAIT_ARGS_NR 16
#define MAX_PROCESS_PAGE 32

#define MAX_SIG_NR 2 // SIGINT, SIGKILL
#define SIG_INT_NUM  0
#define SIG_KILL_NUM 1
#define SIG_INT_MASK  (1 << SIG_INT_NUM)
#define SIG_KILL_MASK (1 << SIG_KILL_NUM)

#define TASK_NEW     0
#define TASK_READY   1
#define TASK_RUNNING 2
#define TASK_BLOCK   3
#define TASK_ZOMBIE  4

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

struct sig_struct {
    unsigned long sigpend;
    unsigned long user_handler[MAX_SIG_NR]; // 0 for undefined
    char *sigaltstack;
    char *ksp;
};

#define VM_ANONYMOUS 0x1 // 1: anoymous; 0: file
#define VM_SHARED    0x2 // 1: shared  ; 0: private
#define PROT_RO      0x1
#define PROT_RW      0x2

struct vm_area_struct {
    unsigned long vm_start;
    unsigned long vm_end;
    unsigned int vm_prot;
    unsigned int vm_flag;
    struct vm_area_struct *vm_next;
};

struct mm_struct {
    struct vm_area_struct *mmap;
    unsigned long *pgd;
    char *cpio_start;
    int cpio_size;
    int page_nr;
    /* TODO: pages[] holds page this process
     * owned. However, in the execution time,
     * process may release some pages(e.g. cow).
     * At that moment, it will leave a hole
     * in pages[i]. A better way is to redesign
     * pages[] as a pool, so we can allocate
     * page from it by finding empty hole.
     */
    struct page *pages[MAX_PROCESS_PAGE];
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
    struct sig_struct sig;
    struct task *next, *prev;
    struct file *fd_tab[8];
    struct vnode *wd;
    struct mm_struct *mm;
};

struct trap_frame {
    unsigned long regs[31];
    unsigned long sp_el0;
    unsigned long elr_el1; // pc
    unsigned long spsr_el1;
};

struct wait_h {
    int free;
    struct task *head;
};

struct wait_args {
    int free;
    struct task *old;
    struct wait_h *waitQueue;
};


int sleep(int seconds);
void init_ts_pool();
struct task *new_ts();
int add_to_ready(struct task *new);
struct task *thread_create(unsigned long func_addr, unsigned long args);
struct task *pick_next();
int schedule();
int sys_exit();
extern void cpu_switch_to(struct task* prev, struct task* next);
int rm_from_ready(struct task *old);
int init_mms_pool();
int init_vma_pool();
