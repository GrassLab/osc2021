#ifndef SCHED_H
#define SCHED_H
#include "data_type.h"
#include "exc.h"
#include "vfs.h"


#define TASK_CONTEXT_SIZE 0x100
#define STACK_SIZE 0x4000
#define task_fd_num 10  // must be bigger than 3
struct task_kcontext {
    u64 x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, fp, lr, sp;
};

struct fd_entry {
    int flag;
    struct file *f;
};

enum FD_FLAG {
    FD_OPEN,
    FD_CLOSE,
};

struct task_struct {
    u32 id;
    struct task_kcontext kcontext;
    void *kstack_top, *stack_top;
    struct task_struct *next;
    float timestamp;
    int flag;
    struct fd_entry fdt[task_fd_num];
};

struct task_queue {
    struct task_struct *head, *tail;
    u32 num;
};

extern struct task_struct *current_task;
extern struct task_queue waiting_queue, unready_queue, running_task,
        suspend_queue;

struct task_struct *task_queue_pop_head (struct task_queue *q);

void init_sched ();
void fork_thread (struct trap_frame *tf);
void create_thread (struct trap_frame *tf, void *addr);
void exec_thread_cpio (struct trap_frame *tf);

void schedule ();
void schedule_wait (unsigned long time);
void schedule_kill ();
void _get_pid (struct trap_frame * tf);
void release_children_thread (struct trap_frame *tf);
struct task_struct *init_task_struct ();

int get_empty_fd ();
struct fd_entry *get_fd_entry (int fd);
#endif
