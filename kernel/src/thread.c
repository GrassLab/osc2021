#include "thread.h"
#include "dynamic_alloc.h"
#include "def.h"
#include "io.h"

struct Thread_pool thread_pool;
int distribute_tid = 0;

void init_thread_pool() {
    thread_pool.head = NULL;
    thread_pool.tail = NULL;
    thread_pool.num_threads = 0;
}

void thread_pool_add(struct Thread *t)
{
    if (thread_pool.num_threads == 0) {
        thread_pool.head = thread_pool.tail = t;
    } else {
        t->prev = thread_pool.tail;
        thread_pool.tail->next = t;
    }


    thread_pool.num_threads++;
}

int get_new_tid()
{
    int tmp = distribute_tid;
    distribute_tid++;
    return tmp;
}

void create_thread(void(*thread_func)())
{
    // create thread structure
    struct Thread *t = malloc(sizeof(struct Thread));
    
    t->tid = get_new_tid();
    t->state = RUNNING;
    t->code = thread_func;

    // // create thread stack
    void * stack = malloc(256);
    t->user_sp = (uint64_t)stack; // 256 for user sp
    t->kernel_sp = (uint64_t)stack + 128; // 256 for kernel sp
    
    // TODO: get pid from parent's pid

    
    distribute_tid++;
}

