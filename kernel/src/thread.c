#include "thread.h"
#include "dynamic_alloc.h"
#include "pf_alloc.h"
#include "def.h"
#include "io.h"
#include "scheduler.h"
#include "math.h"
#include "system_call.h"

struct Thread_pool thread_pool;
int distribute_tid = 1;

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
    t->pid = sys_getpid();
    t->state = RUNNING;
    t->code = thread_func;

    // create and set stack initial value
    alloc_page((void **)&t->kernel_sp, THREAD_STACK_SIZE);
    t->user_sp = t->kernel_sp - int_pow(2, THREAD_STACK_SIZE - 1); // half for user sp
    

    // preserved for register restoring
    t->kernel_sp -= 256;
    struct context *ctx = (struct context *)t->kernel_sp;
    ctx->lr = (uint64_t)thread_func;
    ctx->fp = t->kernel_sp;
    
    // TODO: get pid from parent's pid
    thread_pool_add(t);
    // enqueue into run queue
    enqueue(t);
}

