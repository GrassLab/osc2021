#include "thread.h"

static Thread *run_queue;
static uint64_t next_id = 1;
static Thread_reg idle_reg;

extern void switch_to(Thread_reg *prev, Thread_reg *next, void (*entry)(), void *pgd);
extern Thread_reg *get_current();
extern void fork_stack(void *child_fp, void *parent_fp, uint64_t stack_size);
extern void thread_copy_reg(Thread_reg *reg, void *fp, void (*lr)(), void *sp);
extern uint64_t get_fp();
extern uint64_t get_sp();

void idle(){
    while(1){
        kill_zombies();
        schedule();
    }
}

int64_t thread_create(void (*func)()){
    Thread *new = malloc(sizeof(Thread));
    if(!new){
        return -1;
    }
    new->next = NULL;
    new->pid = next_id++;
    new->ppid = 0;
    new->entry = func;
    new->kernel_stack = palloc(1);
    if(!new->kernel_stack){
        free(new);
        return -1;
    }
    new->user_stack_npage = 0;
    new->user_stack = NULL;
    new->state = 1;
    new->thread_time = 0;
    strfill((char *)new->fd_info, '\0', sizeof(new->fd_info));
    thread_copy_reg(&new->reg, (char *)new->kernel_stack + PAGE_SIZE, exit, (char *)new->kernel_stack + PAGE_SIZE);
    new->pgd = 0;

    Thread *t = current_thread();
    if(t){
        new->ppid = t->pid;
        strncpy(new->pwd, t->pwd, -1);
    }else{
        strncpy(new->pwd, get_pwd(), -1);
    }

    if(!run_queue){
        run_queue = new;
        return new->pid;
    }

    Thread *lastq = run_queue;
    while(lastq->next){
        lastq = lastq->next;
    }
    lastq->next = new;

    return new->pid;
}

void exit(){
    Thread *t = current_thread();
    if(t){
        printf("exit from pid: %ld" NEW_LINE, t->pid);
        t->state = 0;
    }
    schedule();
}

Thread *current_thread(){
    Thread_reg *current_reg = (Thread_reg *)((uint64_t)get_current() | 0xFFFF000000000000);
    Thread *t = (Thread *)((uint64_t)run_queue | 0xFFFF000000000000);
    while(current_reg && ((uint64_t)t != 0xFFFF000000000000)){
        if(&t->reg == current_reg){
            return t;
        }
        t = (Thread *)((uint64_t)t->next | 0xFFFF000000000000);
    }
    return NULL;
}

Thread *get_thread(uint64_t pid){
    Thread *t = run_queue;
    while(t){
        if(t->pid == pid){
            return t;
        }
        t = t->next;
    }
    return NULL;
}

void kill_zombies(){
    Thread *prev = NULL;
    Thread *curr = run_queue;
    Thread *next = NULL;
    while(curr){
        next = curr->next;
        if(curr->state == 0){
            if(!prev){
                run_queue = next;
            }else{
                prev->next = next;
            }
            free(curr->kernel_stack);
            free(curr->user_stack);
            free(curr);
            printf("killed zombie pid: %lu" NEW_LINE, curr->pid);
        }else{
            prev = curr;
        }
        curr = next;
    }
}

void schedule(){
    if(!run_queue){
        return;
    }
    Thread *curr = current_thread();
    if(!curr){
        //printf("idle switch_to %ld" NEW_LINE, run_queue->pid);
        if(run_queue->state == 1){
            run_queue->state = 2;
            switch_to(&idle_reg, &run_queue->reg, run_queue->entry, run_queue->pgd);
        }else{
            switch_to(&idle_reg, &run_queue->reg, NULL, run_queue->pgd);
        }
    }else{
        curr->thread_time = 0;
        Thread *next = curr->next;
        while(next && next->state == 0){
            next = next->next;
        }
        if(!next){
            //printf("%ld switch_to idle" NEW_LINE, curr->pid);
            switch_to(&curr->reg, &idle_reg, NULL, 0);
        }else{
            //printf("%ld switch_to %ld" NEW_LINE, curr->pid, next->pid);
            if(next->state == 1){
                next->state = 2;
                switch_to(&curr->reg, &next->reg, next->entry, run_queue->pgd);
            }else{
                switch_to(&curr->reg, &next->reg, NULL, run_queue->pgd);
            }
        }
    }
}

int64_t fork(){
    Thread *t = current_thread();
    if(!t){
        miniuart_send_S("Warning: No running thread to fork!!!" NEW_LINE);
        return -1;
    }
    Thread *new = malloc(sizeof(Thread));
    if(!new){
        return -1;
    }
    new->next = NULL;
    new->pid = next_id++;
    new->ppid = t->pid;
    new->entry = t->entry;
    new->kernel_stack = palloc(1);
    if(!new->kernel_stack){
        free(new);
        return -1;
    }
    new->user_stack_npage = 0;
    new->user_stack = NULL;
    new->state = t->state;
    new->thread_time = t->thread_time;
    strncpy(new->pwd, t->pwd, -1);
    strncpy((char *)new->fd_info, (char *)t->fd_info, sizeof(new->fd_info));

    fork_stack(new->kernel_stack, t->kernel_stack, PAGE_SIZE);
    if(t->user_stack){
        new->user_stack_npage = t->user_stack_npage;
        new->user_stack = palloc(new->user_stack_npage);
        fork_stack(new->user_stack, t->user_stack, new->user_stack_npage * PAGE_SIZE);
        new->pgd = fork_vm(t, new->kernel_stack, new->user_stack, new->user_stack_npage);
        new->va_kernel = t->va_kernel;
        new->va_user = t->va_user;
    }else{
        new->pgd = vm_user_init(new->kernel_stack, new->user_stack, new->user_stack_npage);
    }
    thread_copy_reg(&new->reg, ((void *)get_fp() - t->kernel_stack) + new->kernel_stack, NULL, ((void *)get_sp() - t->kernel_stack) + new->kernel_stack);

    if(current_thread()->pid == new->pid){
        return 0;
    }

    if(!run_queue){
        run_queue = new;
        return new->pid;
    }

    Thread *lastq = run_queue;
    while(lastq->next){
        lastq = lastq->next;
    }
    lastq->next = new;

    return new->pid;
}
