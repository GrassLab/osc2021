#include "thread.h"

typedef struct thread_queue{
    thread_info *head, *end;
}thread_queue;

thread_queue run_queue;
int thread_cnt;

void init_thread(){
    run_queue.head = run_queue.end = nullptr;
    thread_cnt = 0;
}
thread_info* Thread(void* func){
    thread_info* ptr = (thread_info*)malloc(THREAD_SIZE);
    void *sp_ptr = malloc(PAGE_SIZE);
    ptr->context[10] = (unsigned long)ptr + THREAD_SIZE;
    ptr->context[11] = (unsigned long)func;
    ptr->context[12] = (unsigned long)ptr + THREAD_SIZE;
    ptr->tid = thread_cnt++;
    ptr->status = ALIVE;
    ptr->next = nullptr;
    add_to_run_queue(ptr);
    return ptr;
}
void schedule(){
    if(run_queue.head == nullptr) return;
    if(run_queue.head == run_queue.end) return; // idle thread
    else{
        do{
            run_queue.end->next = run_queue.head;
            run_queue.head = run_queue.head->next;
            run_queue.end = run_queue.end->next;
            run_queue.end->next = nullptr;
        } while(run_queue.head->status != ALIVE);
        switch_to(get_current(), (uint64_t)run_queue.head->context);
    }
}
void add_to_run_queue(thread_info *t){
    if(run_queue.head == nullptr)
        run_queue.head = run_queue.end = t;
    else{
        run_queue.end->next = t;
        run_queue.end = t;
    }
    return;
}

thread_info *current_thread(){
    thread_info *ptr;
    asm volatile("mrs %0, tpidr_el1\n" : "=r"(ptr):);
    return ptr;
}
void exit(){
    // move current thread to wait queue
    thread_info *cur = current_thread();
    cur->status = DEAD;
    schedule();
}
void kill_zombies(){
    thread_info *ptr = run_queue.head;
    while(1){
        while(ptr->next != nullptr && ptr->next->status==DEAD){
            thread_info *tmp = ptr->next->next;
            free((void*)(ptr->next));
            ptr->next = tmp;
        }
        if(ptr->next == nullptr){
            run_queue.end = ptr;
            break;
        }
        else{
            ptr = ptr->next;
        }
    }
}
void foo(){
    for(int i = 0; i < 5; ++i){
        printf("Thread id: %d, %d\n", current_thread()->tid, i);
        delay(1000000);
        schedule();
    }
    exit();
}

void idle(){
    int cnt = 0;
    while(1){
        kill_zombies();
        schedule();
        //uart_puts("in idle\r\n");
        //++cnt;
    }
}
void thread_test(){
    thread_info *idle_t = Thread(nullptr);
    asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)idle_t));
    for(int i = 0; i < 4; ++i){
        Thread(foo);
    }
    idle();
}