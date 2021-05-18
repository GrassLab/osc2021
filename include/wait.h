// Implementation of wait queue is simplified version of linux 5.12
#ifndef _WAIT_H
#define _WAIT_H

#include "list.h"
#include "types.h"

typedef struct wait_queue_entry wait_queue_entry_t;
typedef struct wait_queue_head wait_queue_head_t;
/*
 * Macros for declaration and initialisaton of the datatypes
 */
#define __WAITQUEUE_INITIALIZER(name, tsk) {                \
    .entry      = { NULL, NULL },                           \
    .private    = tsk }

#define DECLARE_WAITQUEUE(name, tsk)                        \
    struct wait_queue_entry name = __WAITQUEUE_INITIALIZER(name, tsk)

#define __WAIT_QUEUE_HEAD_INITIALIZER(name) {               \
    .head       = { &(name).head, &(name).head } }

#define DECLARE_WAIT_QUEUE_HEAD(name)                       \
    struct wait_queue_head name = __WAIT_QUEUE_HEAD_INITIALIZER(name)

struct wait_queue_head {
    struct list_head head;
};

struct wait_queue_entry {
    struct list_head entry;
    void *private; // point to task_struct
};

extern struct wait_queue_head uart_read_waitQueue;


void add_wait_queue(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry);
void remove_wait_queue(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry);
void dump_wait_queue(struct wait_queue_head *wq_head);

pid_t wait(struct wait_queue_head *wq_head);
void wake_up(struct wait_queue_head *wq_head);
static inline void 
__add_wait_queue(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry)
{
    list_add(&wq_entry->entry, &wq_head->head);
}

static inline void __add_wait_queue_entry_tail(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry)
{
	list_add_tail(&wq_entry->entry, &wq_head->head);
}


static inline void
__remove_wait_queue(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry)
{
    list_del(&wq_entry->entry);
}

#endif /* _WAIT_H */
