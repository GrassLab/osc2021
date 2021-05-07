#include "mini_uart.h"
#include "memory.h"
#include "thread.h"

void sys_exec(char* name, char** argv);
extern uint64_t total_threads;
extern uint64_t pid_counter;
extern MemAlloc allocator;

extern"C"
void kmain() {
    char* tmp[] = {
        "hello", "This is the test parameter 1", "This is the test parameter 2", nullptr
    };
    MiniUART::Init();
    io() << "Kernel started!!\r\n";
    allocator.Init();
    tasks[0].program_alloc = malloc(4096);
    tasks[0].stack_alloc = malloc(4096);
    tasks[0].kernel_stack_alloc = malloc(4096);
    tasks[0].pid = ++pid_counter;
    total_threads++;
    sys_exec("hello", tmp);
}
