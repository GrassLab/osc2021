#include "scheduler.h"
#include "sched.h"
#include "allocator.h"
#include "utils.h"
#include "uart.h"
#include "cpio.h"
#include "vfs.h"

#define CREATE_SUCCESS 0
#define CREATE_FAIL    1

//static thread init_thread = INIT_THREAD;

thread *run_queue[MAX_THREAD_COUNT] = {NULL};
thread *current = NULL; // point to current thread
int curr_index = -1; // current thread index in queue
int thread_count = 0; // There is no thread at the begining

static void preempt_disable(void) {
    if(current) current->preempt_flag++;
}

static void preempt_enable(void) {
	if(current) current->preempt_flag--;
}

void delay(unsigned long count) {
    while(--count);
}

void switch_to(thread *next) {
    //if(current == next) return;
    thread *prev = current;
    current = next;
    cpu_switch_to(prev, next);
}

pt_regs* task_pt_regs(thread *tsk){
	unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(pt_regs);
	return (pt_regs *)p;
}


int create_thread(unsigned long clone_flags, unsigned long func, unsigned long arg, unsigned long stack) {
    preempt_disable();
    thread *p;
    
    // allocate 1 page(4KB) for each thread
    p = (thread*) kmalloc(THREAD_SIZE);
    if(!p) return CREATE_FAIL;

	if (clone_flags & PF_KTHREAD) {
		p->cpu_context.x20 = arg;
        p->cpu_context.sp = (unsigned long)p + 4096;
        p->cpu_context.fp = (unsigned long)p + 4096;
        p->cpu_context.lr = func;
        for(int i = 0; i < FD_MAX_SIZE; i++) {
            p->fd_table[i] = 0;
        }
        p->child = 0;
	} 
    else {
        // clone
        /*
        unsigned char *dest = (unsigned char *)p;
        unsigned char *src = (unsigned char *)current;
        for(int i = 0; i < 4096; i++) {
            dest[i] = src[i];    
        }
        */
        memcpy(p, current, 4096);
        for(int i = 0; i < FD_MAX_SIZE; i++) {
            p->fd_table[i] = current->fd_table[i];
        }

        unsigned long k_delta = (unsigned long)p - (unsigned long)current;
        p->cpu_context.ux0 = 0;
        p->cpu_context.fp += k_delta;
        p->cpu_context.sp += k_delta;
        p->cpu_context.elr += (unsigned long)0x100000;
        p->cpu_context.usp += (unsigned long)0x100000;
        p->cpu_context.ufp += (unsigned long)0x100000;
        p->cpu_context.ulr += (unsigned long)0x100000;
        //p->cpu_context.lr = p->cpu_context.ulr;

        /*
        char *src_stack = (char *)current->cpu_context.usp;
        char *dest_stack = (char *)p->cpu_context.usp;
        for(int i = 0; i < 4096; i++) {
            dest_stack[i] = src_stack[i];
        }
        */
        memcpy(0x300000, 0x200000, 0x100000);
    }

    p->id = thread_count++;
    p->state = READY;
    p->priority = 2;
    p->counter = p->priority;
    current->child = 1;
    
    run_queue[p->id] = p;
    preempt_enable();
    
    // parent
    return p->id;
}


void scheduler() {
    preempt_disable();
    int next_index;
    thread *p = NULL;
    int find_next_thread = 0;
    if(current && current->state != DEAD) {
        current->state = READY;
        current->counter = 0;
    }

    // Find next available thread (state=READY and its counter > 0), searching from current index + 1
    for(int i = 0; i < thread_count; i++) {
        next_index = (curr_index+1) % thread_count;
        p = run_queue[next_index];
        if(p && p->state == READY && p->counter > 0) {
            find_next_thread = 1;
            break;
        }
    }
    // When all thread has no time or no available thread can run, dispatch time for each thread then start from thread 0
    if(!find_next_thread) {
        for(int i = 0; i < thread_count; i++) {
            p = run_queue[i];
            p->counter = (p->counter >> 1) + p->priority;
        }
        next_index = 0;
    }
    
    run_queue[next_index]->state = RUNNING;
    curr_index = next_index;
    switch_to(run_queue[next_index]);
    preempt_enable();
}

void schedule_tail(void) {
	preempt_enable();
}

void timer_tick() {
    --current->counter;
    if(current->counter > 0) return;
    current->counter = 0;
    uart_puts("timer tick\n");
    scheduler();
}

void _exit() {
    preempt_disable();
    current->state = DEAD;
    uart_puts("thread ");
    uart_puts_int(current->id);
    uart_puts(" is dead\n");
    preempt_enable();
    scheduler();
}

// remove the DEAD thread from queue and relocate the rest thread
static void relocate_queue() {
    for(int i = 0; i < thread_count; i++) {
        thread *dest = run_queue[i];
        if(dest->state == DEAD) {
            uart_puts("relocate...\n");
            for(int j = i+1; j < MAX_THREAD_COUNT; j++) {
                thread *src = run_queue[j];
                if(src && src->state != DEAD) {
                    run_queue[i] = src;
                    run_queue[j] = NULL;
                }
            }
        }
    }
    for(int i = 0; i < MAX_THREAD_COUNT; i++) {
        thread *p = run_queue[i];
        if(p && p->state==DEAD) run_queue[i] = NULL;
    }
}

void idle() {
    // kill zombies
    while(1) {
        preempt_disable();
        for(int i = 0; i < MAX_THREAD_COUNT; i++) {
            thread *p = run_queue[i];
            if(p && p->state == DEAD) {
                for(int i = 0; i < FD_MAX_SIZE; i++) {
                    if(p->fd_table[i]) {
                        vfs_close(p->fd_table[i]);
                    }
                }
                free_page((unsigned long)p, THREAD_SIZE);
                thread_count--;
            }
        }
        relocate_queue();
        preempt_enable();
        uart_puts("I am zombie killer, remain ");
        uart_puts_int(thread_count);
        uart_puts(" threads\n");
        delay(1000000);
        if(current->child != 0) {
            create_thread(0, 0, 0, 0);
        }
        scheduler();
    }
}

void _exec(char *name, char **argv) {
    unsigned long ret = load_user_program_withArgv(name, argv);
    if(!ret) {
        uart_puts("load user program fail...\n");
        _exit();
        return;
    }
    else {
        uart_puts("load user program success...\n");
    }
    //run_queue[current->id]->cpu_context.pc = ret;
    
    //_exit();
}