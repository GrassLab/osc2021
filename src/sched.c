#include "sched.h"
#include "mm.h"
#include "context.h"
#include "uart.h"

#define MAX_THREAD_NUM 100
#define TSTACK_SIZE    2048

typedef struct {
    list_head_t head;
} thread_queue_t;

thread_queue_t run_queue;
thread_queue_t dead_queue;
thread_queue_t wait_read_queue;
thread_queue_t wait_queue;
thread_t thread_pool[MAX_THREAD_NUM];

bool_t resched_flag;
size_t read_size;
int remaining_time;

/*
 * Thread relate function
 */
static void reg_init(callee_reg_t *reg) {
    reg->x19 = 0;
    reg->x20 = 0;
    reg->x21 = 0;
    reg->x22 = 0;
    reg->x23 = 0;
    reg->x24 = 0;
    reg->x25 = 0;
    reg->x26 = 0;
    reg->x27 = 0;
    reg->x28 = 0;
    reg->fp = 0;
    reg->lr = 0;
    reg->sp = 0;
}

static void thread_init(void) {
      for (int i = 0; i < MAX_THREAD_NUM; i++) {
          thread_pool[i].pid = i;
          thread_pool[i].kstack_addr = NULL;
          for (int j = 0; j < MAX_USTACK_NUM; j++)
              thread_pool[i].ustack_addr[j] = NULL;
          thread_pool[i].ustack_num = 0;
          thread_pool[i].read_size = 0;
          thread_pool[i].wait_time = 0;
          for (int j = 0; j < MAX_FD_NUM; j++)
              thread_pool[i].fd_table[j] = NULL;
          thread_pool[i].state = exit;
          reg_init(&thread_pool[i].reg);
          list_init(&thread_pool[i].node);
      }
      thread_pool[0].state = running; /* Startup thread */
      tpidr_el1_init(&thread_pool[0]);
}

thread_t* thread_create(void (*task)(void)) {
    uint16_t count = 0;
    while(count < MAX_THREAD_NUM && thread_pool[++count].state != exit) {}
    if (count == MAX_THREAD_NUM)
        return NULL;
    thread_pool[count].kstack_addr = kmalloc(TSTACK_SIZE);
    thread_pool[count].wd = rootfs->root;
    thread_pool[count].fd_pos = 0;
    thread_pool[count].state = ready;
    /* Set context */
    thread_pool[count].reg.sp = (uint64_t)thread_pool[count].kstack_addr + TSTACK_SIZE;
    thread_pool[count].reg.fp = (uint64_t)thread_pool[count].kstack_addr + TSTACK_SIZE;
    thread_pool[count].reg.lr = (uint64_t)task;
    list_add_tail(&run_queue.head, &thread_pool[count].node);
    return &thread_pool[count];
}

void thread_exit(void) {
    thread_t *current = get_current();
    current->state = dead;
    list_add_tail(&dead_queue.head, &current->node);
    schedule();
}

/*
 * Scheduler
 */
void sched_init(void) {
    resched_flag = false;
    read_size = 0;
    remaining_time = -1;
    list_init(&run_queue.head);
    list_init(&dead_queue.head);
    list_init(&wait_read_queue.head);
    list_init(&wait_queue.head);
    thread_init();
}

static void context_switch(thread_t *next) {
    thread_t *current = get_current();
    next->state = running;
    list_del(run_queue.head.next);
    if (current->state == wait) {
        if (current->read_size) {
            if (list_empty(&wait_read_queue.head) == true)
                read_size = current->read_size;
            list_add_tail(&wait_read_queue.head, &current->node);
        } else {
            list_add_tail(&wait_queue.head, &current->node);
            remaining_time = 8;
        }
    } else if (current->state == running) {
        current->state = ready;
        list_add_tail(&run_queue.head, &current->node);
    }
    switch_to(current, next);
}

static void kill_zombie(void) {
    while (list_empty(&dead_queue.head) == false) {
        thread_t *tmp = list_entry(dead_queue.head.next, thread_t, node);
        list_del(dead_queue.head.next);
        kfree(tmp->kstack_addr);
        for (int i = 0; i < MAX_FD_NUM; i++) {
            if (tmp->fd_table[i] != NULL) {
                vfs_close(tmp->fd_table[i]);
                tmp->fd_table[i] = NULL;
            }
        }
        tmp->state = exit;
        reg_init(&tmp->reg);
    }
}

void schedule(void) {
    if (list_empty(&run_queue.head) == false) {
        thread_t *next = list_entry(run_queue.head.next, thread_t, node);
        context_switch(next);
    }
}

void idle(void) {
     while (1) {
        kill_zombie();
        schedule();
    }
}

int wake_up(uint16_t mode) {
    if (!mode) { /* Uart interrupt */
        thread_t *tmp = list_entry(wait_read_queue.head.next, thread_t, node);
        list_del(wait_read_queue.head.next);
        tmp->state = ready;
        tmp->read_size = 0;
        list_add_tail(&run_queue.head, &tmp->node);
        if (list_empty(&wait_read_queue.head) == true) {
            return -1;
        } else {
            thread_t *next = list_entry(wait_read_queue.head.next, thread_t, node);
            read_size = next->read_size;
        }
    } else { /* Timer interrupt */
        list_head_t *pos = wait_queue.head.next;
        while (pos != &wait_queue.head) {
            thread_t *tmp = list_entry(wait_queue.head.next, thread_t, node);
            tmp->wait_time--;
            pos = pos->next;
            if (!tmp->wait_time) {
                tmp->state = ready;
                list_del(&tmp->node);
                list_add_tail(&run_queue.head, &tmp->node);
            }
        }
        if (list_empty(&wait_queue.head) == false)
            remaining_time = 8;
    }
    return 0;
}

void times_up(void) {
    if (resched_flag == true) {
        resched_flag = false;
        schedule();
    }
}
