#include "thread.h"

thread_info *run_queue, *wait_queue;
void *kernel_tpdir1_el1;


void init_thread(){
    run_queue = wait_queue = nullptr;
    //Thread(idle);
    //Thread(shell);
    // schedule();
}
void Thread(void* func){
    thread_info* ptr = malloc(sizeof(thread_info));
    void *sp_ptr = malloc(PAGE_SIZE);
    *ptr = (thread_info){
        .prev = nullptr,
        .next = nullptr,
        .reg = {},
        .fp = sp_ptr,
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
    switch_to((void*)get_current(), run_queue->reg);
}
void add_to_run_queue(thread_info *t){
    t->prev = t;
    t->next = t;
    if(run_queue != nullptr){
        t->next = run_queue;
        t->prev = run_queue->prev;
        run_queue->prev->next = t;
        run_queue->prev = t;
    }
    run_queue = t;
}
void move_to_wait_queue(){
    thread_info *cur = run_queue;
    if(run_queue == run_queue->next) run_queue = nullptr;
    else run_queue = run_queue->next;
    cur->next->prev = cur->prev;
    cur->prev->next = cur->next;
    cur->next = cur->prev = cur;
    if(wait_queue != nullptr){
        cur->next = wait_queue;
        cur->prev = wait_queue->prev;
        wait_queue->prev->next =cur;
        wait_queue->prev = cur; 
    }
    cur->status = DEAD;
    wait_queue = cur;
}
int available_thread_id(){
    // find unused thread id in run queue and wait queue
    char flag[1024] = {};
    if(run_queue != nullptr){
        thread_info* ptr = run_queue;
        do{
            flag[ptr->tid] = 1;
            ptr = ptr->next;
        }while(ptr != run_queue);
    }
    if(wait_queue != nullptr){
        thread_info* ptr = wait_queue;
        do{
            flag[ptr->tid] = 1;
            ptr = ptr->next;
        }while(ptr != wait_queue);
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
void exit(){
    // move current thread to wait queue
    thread_info* next_thread = run_queue->next;
    if(next_thread == run_queue) return;
    move_to_wait_queue();
    schedule();
}
void kill_zombies(){
    if(wait_queue == nullptr) return;
    thread_info *ptr = wait_queue;
    do{
        if(ptr->status == DEAD){
            ptr->prev->next = ptr->next;
            ptr->next->prev = ptr->prev;
            thread_info *tmp;
            if(ptr->next == ptr) tmp = nullptr;
            else tmp = ptr->next;
            free(ptr->fp);
            free(ptr);
            ptr = tmp;
        }
        else
            ptr = ptr->next;
    }while(ptr != nullptr && ptr != wait_queue);
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
    int cnt = 0;
    while(cnt < 10){
        kill_zombies();
        schedule();
        uart_puts("in idle\r\n");
        ++cnt;
    }
}