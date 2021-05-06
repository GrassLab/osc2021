#include "wait.h"
#include "printf.h"
#include "list.h"
#include "types.h"
#include "sched.h"

DECLARE_WAIT_QUEUE_HEAD(uart_read_waitQueue);

void add_wait_queue_entry_tail(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry)
{
    __add_wait_queue_entry_tail(wq_head, wq_entry);
}

void remove_wait_queue(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry)
{
    __remove_wait_queue(wq_head, wq_entry);
}

pid_t wait(struct wait_queue_head *wq_head)
{
    // Set current task as waiting sate
    current->state = TASK_WAITING;
    
    // Add current task to wait queue
    DECLARE_WAITQUEUE(waiting_task, current);
    add_wait_queue_entry_tail(wq_head, &waiting_task);
    // printf("[wait] Wait queue state after wait:\n");
    // dump_wait_queue(wq_head);
    printf("[wait] Tasks state after wait:\n");
    dumpTasksState();
    // reschedule
    schedule();
    return current->pid;
}

// This function will wake up all processes that are waiting on this event queue.
void wake_up(struct wait_queue_head *wq_head)
{
    //printf("[wake_up] wake_up!\n");
    struct list_head *pos;
    list_for_each(pos, &wq_head->head) {
        struct task_struct *tk = (struct task_struct *)((wait_queue_entry_t *)pos)->private;
        tk->state = TASK_RUNNING;
        remove_wait_queue(wq_head, (wait_queue_entry_t *)pos);
    }
    //printf("[wake_up] Wait queue state after wake up:\n");
    //dump_wait_queue(wq_head);
}


void dump_wait_queue(struct wait_queue_head *wq_head)
{
    printf("===dump wait queue===\n");
    struct list_head *pos;
    list_for_each(pos, &wq_head->head) {
        struct task_struct *tk = (struct task_struct *)((wait_queue_entry_t *)pos)->private;
        printf("pid = %d -> ", tk->pid);
    }
    printf("\n");
}