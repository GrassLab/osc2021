
# define TASK_MAX_NUM    100
# define TASK_EPOCH      5
# define STACK_SIZE      4096
# define STACK_TOP_IDX   (4096-16)
# define PRIORITY_MAX    10
# define FD_MAX_NUM      50

# define USER_PRO_LR_START   0x80000
# define USER_PRO_SP_START   (0x100000000-0x10)   //4G-16B

enum task_state{
  RUNNING,
  ZOMBIE,
  EXIT,
};

enum task_el{
  USER,
  KERNEL,
};

struct task{
  unsigned long long x19;
  unsigned long long x20;
  unsigned long long x21;
  unsigned long long x22;
  unsigned long long x23;
  unsigned long long x24;
  unsigned long long x25;
  unsigned long long x26;
  unsigned long long x27;
  unsigned long long x28;
  unsigned long long fp;  // x29
  unsigned long long lr;  // x30
  unsigned long long sp;
  unsigned long long ttbr0;
  int pid;
  enum task_state state;
  int priority;
  int counter;
  int resched_flag;
  void (*invoke_func)();
  enum task_el mode;
  struct vnode *pwd_vnode;
  struct file *fd[FD_MAX_NUM];
  struct task *pre;
  struct task *next;
};

void task_init();
void task_exit();
int task_create(void (*func)(), int priority, enum task_el mode);
int privilege_task_create(void (*func)(), int priority);
int user_task_create(char *pathname, int priority);
void yield();
void schedule();
int get_pid();
int get_new_fd(struct file *new_file);
struct file* get_file_by_fd(int fd);
void remove_fd(int fd);
void sys_fork(struct trapframe* trapframe);
void sys_exec(struct trapframe* trapframe);

extern "C" struct task* get_current();
extern "C" void switch_to(struct task *pre, struct task *next);
extern "C" void return_from_fork();
extern "C" void update_pgd(unsigned long long ttbr0);
