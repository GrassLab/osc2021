#include "thread.h"

thread_info *run_queue, *wait_queue;
void *kernel_tpdir1_el1;

void Thread(void* func){
    thread_info* ptr = malloc(sizeof(thread_info));
    void *sp_ptr = malloc(PAGE_SIZE);
    *ptr = (thread_info){
        .prev = nullptr,
        .next = nullptr,
        .reg = {},
        .tp = sp_ptr,
        .lr = func,
        .sp = sp_ptr,
        .pid = 0,
        .tid = available_thread_id(),
        .status = ALIVE
    };
    add_to_run_queue(ptr);
}
void schedule(){
    if(run_queue == nullptr) return;
    run_queue = run_queue->next;
    run_queue->prev->status = ALIVE;
    run_queue->status = ACTIVE;
    switch_to(get_current(), run_queue->reg);
}
void add_to_run_queue(thread_info *t){
    t->prev = t;
    t->next = t;
    if(run_queue != nullptr){
        t->next = run_queue;
        t->prev = run_queue->prev;
        run_queue->prev->next = t;
        run_queue-> = t;
    }
    run_queue = t;
}
int available_thread_id(){
    // find unused thread id in run queue and wait queue
    char flag[1024] = {0};
    if(run_queue != nullptr){
        thread_info* ptr = run_queue;
        do{
            flag[ptr->tid] = 1;
            ptr = ptr->next;
        }while(ptr != run_queue)
    }
    if(wait_queue != nullptr){
        thread_info* ptr = wait_queue;
        do{
            flag[ptr->tid] = 1;
            ptr = ptr->next;
        }while(ptr != wait_queue)
    }
    for(int i = 0 ; i < 1024; ++i){
        if(flag[i] == 0) return i;
    }
    return -1;
}
thread_info *current_thread(){
    if(run_queue == nullptr) return nullptr;
    thread_info* ptr = run_queue;
    while(ptr != nullptr && ptr->status != ACTIVE)
        ptr = ptr->next;
    return ptr;
}
void foo(){
    for(int i = 0; i < 10; ++i){
        uart_puts("Thread id: ");
        uart_printint(current_thread()->tid);
        uart_puts(", ");
        uart_printint(i);
        uart_puts("\r\n");
        delay(1000000);
        schedule();
    }
    exit();
}

void idle(){
    while(true){
        kill_zombie();
        schedule();
    }
}