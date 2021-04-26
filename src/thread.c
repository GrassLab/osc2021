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
    // void *sp_ptr = malloc(PAGE_SIZE);
    ptr->context[10] = (unsigned long)ptr + THREAD_SIZE;
    ptr->context[11] = (unsigned long)func;
    ptr->context[12] = (unsigned long)ptr + THREAD_SIZE;
    ptr->tid = thread_cnt++;
    ptr->status = 0;
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
        } while(run_queue.head->status != 0);
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
unsigned long get_pid(){
    return current_thread()->tid;
}
void exit(){
    // move current thread to wait queue
    thread_info *cur = current_thread();
    cur->status |= THREAD_DEAD;
    schedule();
}
void kill_zombies(){
    if(run_queue.head == nullptr) return;
    for(thread_info* ptr = run_queue.head; ptr->next != nullptr ; ptr = ptr->next){
        for(thread_info *cur = ptr->next; cur != nullptr && (cur->next->status & THREAD_DEAD) > 0;){
            thread_info *tmp = cur->next;
            free((void*)cur);
            ptr->next = tmp;
            cur = tmp;
        }
        if(ptr->next == nullptr){
            run_queue.end = ptr;
            break;
        }
    }
    
}
unsigned long pass_argument(char** argv,unsigned long addr){
    int argc = 0, byte_cnt = 0;
    for(int i = 0;;++i){
        ++argc;
        if(argv[i] == 0) break;
        for(int j = 0;; ++j){
            ++byte_cnt;
            if(argv[i][j] == 0) break;
        }
    }
    // printf("argc: %d %d\n", argc, byte_cnt);
    int arg_size = (1 + 1 + argc) * 8 + byte_cnt;
    addr = addr - arg_size;
    addr = addr - (addr & 15); //sp
    char* data = (char*) addr;
    *(unsigned long*)data = argc - 1; // exclude NULL
    data += 8;
    *(unsigned long*)data = (unsigned long)(data + 8); //argv addr
    data += 8;
    char* argv_buf = data + 8 * argc;
    for(int i = 0; i < argc - 1; ++i){ // without NULL
        *(unsigned long*)data = (unsigned long)argv_buf;
        for(int j = 0;;++j){
            *argv_buf = argv[i][j];
            ++argv_buf;
            if(argv[i][j] == 0) break;
        }
        data += 8;
    }
    *(unsigned long*)data = 0;
    return addr;
}
void load_program_with_args(char* file_name, char**argv, unsigned long addr){
    struct cpio_newc_header *cpio_addr = find_cpio_entry(file_name);
    struct cpio_size_info size_info;
    if(cpio_addr == nullptr){
        printf("file doesn't exist\n");
        return;
    }
    extract_header(cpio_addr, &size_info);
    char* context_addr = (char*)cpio_addr + 110 + size_info.name_size + size_info.name_padding;

    unsigned long context_size = size_info.file_size;
    char* target = (char*) addr;
    for(unsigned long i = 0; i < context_size; ++i, target++, context_addr++){
        *target = *context_addr;
    }
    unsigned long sp_addr = pass_argument(argv, addr);
    printf("load program\n");
    // core_timer_enable();
    uart_printhex(addr); printf("\n");
    uart_printhex(sp_addr); printf("\n");
    _load_user_program((void*)addr, (void*)sp_addr);
}

void exec(char* file_name, char** argv){
    unsigned long addr = USER_PROGRAM_ADDR; // need handle ??
    load_program_with_args(file_name, argv, addr);
    printf("exec done\n");
    exit();
}
int fork(){
    // return child pid
    run_queue.head->status |= THREAD_FORK;
    schedule();
    return run_queue.head->child_pid;
}
void copy_program(thread_info* parent, thread_info* child){
    parent->status ^= THREAD_FORK;
    parent->child_pid = child->tid;

}
void do_fork(){
    for(thread_info* ptr = run_queue.head->next; ptr != nullptr; ptr = ptr->next){
        if(ptr->status & THREAD_FORK){
            thread_info* child = Thread(nullptr);
            copy_program(ptr, child);
        }
    }
}
void idle(){
    int cnt = 0;
    while(1){
        kill_zombies();
        do_fork();
        schedule();
    }
}
void foo(){
    for(int i = 0; i < 5; ++i){
        printf("Thread id: %d, %d\n", current_thread()->tid, i);
        delay(100000000);
        schedule();
    }
    exit();
}
void thread_test(){
    thread_info *idle_t = Thread(nullptr);
    asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)idle_t));
    for(int i = 0; i < 4; ++i){
        Thread(foo);
    }
    idle();
}
void exec_test(){
    char* argv[] = {"argv_test", "-o", "arg2", 0};
    //char* argv[] = {};
    // exec("user_program.img", argv);
    exec("argv_test", argv);
}
void thread_test2(){
    thread_info *idle_t = Thread(nullptr);
    asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)idle_t));
    Thread(exec_test);
    idle();
}