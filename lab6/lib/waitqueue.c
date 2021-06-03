#include <list.h>
#include <current.h>
#include <mm.h>
#include <waitqueue.h>
#include <preempt.h>
#include <printf.h>

struct node {
    struct task_struct *ts;
    struct node *next;
};

struct waitqueue {
    struct node *list;
};

struct waitqueue *alloc_waitqueue() {
    struct waitqueue *wq = kmalloc(sizeof(struct waitqueue));
    wq->list = NULL;

    return wq;
}

void wait(struct waitqueue *wq) {
    struct node *node = kmalloc(sizeof(struct node));
    node->ts = current;

    size_t flags = disable_irq_save();
    node->next = wq->list;
    wq->list = node;
    irq_restore(flags);

    pause_task(current);

    schedule();
}

void wakeup(struct waitqueue *wq) {
    disable_preempt();

    struct node *p = wq->list;
    if (p) {
        struct task_struct *ts = p->ts;
        restart_task(ts);

        wq->list = p->next;
        kfree(p);
    }

    enable_preempt();
}