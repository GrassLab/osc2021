#include "fs.h"
#include "stat.h"
#include "sched.h"
#include "mm.h"
#include "string.h"

void exec_binary(struct file *f) {
    if (S_IXUSR | f->f_mode) {
        void *prog = kmalloc(f->size);
        memcpy(prog, f->data, f->size);

        struct task_struct *ts = kmalloc(sizeof(ts));

        /* TODO: protect page with virtual memory */
        /* TODO: use schedule_task() to do scheduling */
        ts->stack = kmalloc(USTACK_SIZE);
        ts->kstack = kmalloc(KSTACK_SIZE);
        ts->pid = 0;
        ts->user_prog = prog;

        run_task(ts);
    }
}