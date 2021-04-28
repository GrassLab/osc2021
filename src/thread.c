#include "thread.h"
#include "shell.h"

typedef struct thread_queue{
    thread_info *head, *end;
}thread_queue;

thread_queue run_queue;
process_address_generator p_addr_generator;
int thread_cnt, thread_pflag;

void init_thread(){
    run_queue.head = run_queue.end = nullptr;
    thread_cnt = 0;
    for(int i = 0; i < 10; ++i)
        p_addr_generator.addr[i] = USER_PROGRAM_ADDR + 0x400000 * i;
    //p_addr_generator.addr[1] = USER_PROGRAM_ADDR + 0x400000; 
    p_addr_generator.current = 0;
    thread_pflag = 0;
    // thread_info *idle_t = Thread(shell);
    // asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)idle_t));
    // schedule();
}
thread_info* Thread(void* func){
    thread_info* ptr = (thread_info*)malloc(THREAD_SIZE);
    uart_printhex((unsigned long)ptr); uart_puts("\r\n");
    // void *sp_ptr = malloc(PAGE_SIZE);
    ptr->context[10] = (unsigned long)ptr + THREAD_SIZE;
    ptr->context[11] = (unsigned long)func;
    ptr->context[12] = (unsigned long)ptr + THREAD_SIZE;
    ptr->tid = thread_cnt++;
    ptr->status = 0;
    ptr->next = nullptr;
    ptr->p_addr = ptr->p_size = ptr->child_pid = 0;
    add_to_run_queue(ptr);
    return ptr;
}
void threadSwitch(){
	asm volatile("\
_threadSwitch:\n\
		stp x19, x20, [x0, 16 * 0]\n\
		stp x21, x22, [x0, 16 * 1]\n\
		stp x23, x24, [x0, 16 * 2]\n\
		stp x25, x26, [x0, 16 * 3]\n\
		stp x27, x28, [x0, 16 * 4]\n\
		stp x29, x30, [x0, 16 * 5]\n\
		mov x9, sp\n\
		str x9, [x0, 16 * 6]\n\
		\n\
		ldp x19, x20, [x1, 16 * 0]\n\
		ldp x21, x22, [x1, 16 * 1]\n\
		ldp x23, x24, [x1, 16 * 2]\n\
		ldp x25, x26, [x1, 16 * 3]\n\
		ldp x27, x28, [x1, 16 * 4]\n\
		ldp x29, x30, [x1, 16 * 5]\n\
		ldr x9, [x1, 16 * 6]\n\
		mov sp, x9\n\
		\n\
		msr tpidr_el1, x1\n\
		\n\
		ret\n\
	"::);
}

void schedule(){
    if(run_queue.head == nullptr) {
    
        return;
    }
    if(run_queue.head == run_queue.end) {
        // uart_puts("??\r\n");
        free(run_queue.head);
        run_queue.head = run_queue.end = nullptr;
        thread_cnt = 0;
        thread_pflag = 0;
        return; // idle thread
    }
    else{
        do{
            run_queue.end->next = run_queue.head;
            run_queue.end = run_queue.head;
            run_queue.head = run_queue.head->next;
            run_queue.end->next = nullptr;
        } while(run_queue.head->status);
        switch_to(get_current(), (unsigned long)(run_queue.head->context));
        return;
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
    // printf("thread %d marked as DEAD\n", cur->tid);
    schedule();
    return;
}
void kill_zombies(){
    if(run_queue.head == nullptr) return;
    for(thread_info* ptr = run_queue.head; ptr->next != nullptr ; ptr = ptr->next){
        for(thread_info *cur = ptr->next; cur != nullptr && ((cur->status) & THREAD_DEAD) > 0;){
            thread_info *tmp = cur->next;
            //printf("find dead thread %d\n", cur->tid);
            free((void*)cur);
            ptr->next = tmp;
            cur = tmp;
        }
        if(ptr->next == nullptr){
            run_queue.end = ptr;
            break;
        }
    }
    //printf("leave kill_zombie\n");
    return;
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
void load_program_with_args(char* file_name, char**argv, unsigned long addr, thread_info* current_thread){
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
    current_thread->p_addr = addr;
    current_thread->p_size = context_size;
    //printf("load program\n");
    // core_timer_enable();
    //uart_printhex(addr); printf("\n");
    //uart_printhex(sp_addr); printf("\n");
    _load_user_program((void*)addr, (void*)sp_addr);
}
unsigned long __generate_user_addr(){
    unsigned long res = p_addr_generator.addr[p_addr_generator.current];
    p_addr_generator.current++;
    return res;
}
void exec(char* file_name, char** argv){
    unsigned long addr = __generate_user_addr(); // need handle ??
    load_program_with_args(file_name, argv, addr, run_queue.head);
    //printf("exec done\n");
    exit();
}
int fork(){
    run_queue.head->status |= THREAD_FORK;
    schedule();
    return run_queue.head->child_pid;
}
void copy_program(thread_info* parent, thread_info* child){
    //uart_puts("copy\r\n");
    parent->status ^= THREAD_FORK;
    parent->child_pid = child->tid;
    thread_info *child_next = child->next;
    uint64_t org_tid = child->tid;
    char* src = (char*) parent;
    char* dst = (char*) child;
    for(int i = 0; i < THREAD_SIZE; ++i, ++src, ++dst){
        *dst = *src;
    }
    child->next = child_next;
    child->child_pid = 0;
    child->tid = org_tid;
    child->p_addr = __generate_user_addr();
    long reg_addr_diff = (long)child - (long)parent;
    long p_diff = (long)child->p_addr - (long)parent->p_addr;
    child->context[10] += reg_addr_diff; // fp
    child->context[12] += reg_addr_diff; // sp
    child->context[14] += p_diff; //elr_el1
    child->context[15] += p_diff; //sp_el0
    child->context[45] += p_diff;
    child->context[46] += p_diff;
    // copy stack
    // uart_printhex(parent->p_addr); uart_puts("\r\n");
    // uart_printhex(child->p_addr); uart_puts("\r\n");
    // uart_printhex(parent->p_addr); uart_puts("\r\n");
    // uart_printhex(parent->p_size); uart_puts("\r\n");
    // uart_printhex(parent->context[15]); uart_puts("\r\n");
    src = (char*) (parent->context[15]);
    dst = (char*) (child->context[15]);
    long st_size = (parent->p_addr) + parent->p_size - parent->context[15];
    for(int i = 0; i < st_size; ++i, ++dst, ++src){
        *dst = *src;
    }
    printf("copy done\n");
    return;
}
void do_fork(){
   // printf("do_fork\n");
    for(thread_info* ptr = run_queue.head->next; ptr != nullptr; ptr = ptr->next){
        if((ptr->status) & THREAD_FORK){
            thread_info* child = Thread(nullptr);
            copy_program(ptr, child);
            // schedule();
        }
    }
    return;
}
void idle(){
    int cnt = 0;
    while(thread_pflag){
        kill_zombies();
        do_fork();
        schedule();
    }
    return;
}
void foo(){ 
    for(int i = 0; i < 5; ++i){
        //uart_printint(get_pid());
        printf("Thread id: %d, %d\r\n", current_thread()->tid, i);
        delay(1000000);
        schedule();
    }
    exit();
    return;
}
void thread_test(){
    thread_pflag = 1;
    thread_info *idle_t = Thread(nullptr);
    asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)idle_t));
    for(int i = 0; i < 4; ++i){
        Thread(foo);
    }
    //printf("before idle\n");
    idle();
    return;
}
void exec_test(){
    char* argv[] = {"argv_test", "-o", "arg2", 0};
    //char* argv[] = {};
    // exec("user_program.img", argv);
    exec("argv_test", argv);
    //exec("fork_test", argv);
    return;
}
void thread_test2(){
    thread_pflag = 1;
    thread_info *idle_t = Thread(nullptr);
    asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)idle_t));
    Thread(exec_test);
    idle();
    return;
}