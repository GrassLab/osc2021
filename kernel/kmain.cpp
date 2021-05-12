#include <mini_uart.h>
#include <memory.h>
#include <thread.h>

void sys_exec(char* name, char** argv);
extern uint64_t total_threads;
extern uint64_t pid_counter;
extern MemAlloc allocator;
task_struct *tasks;

extern"C"
void kmain() {
    char* tmp[] = {
        "terminal", nullptr
    };
    tasks = (task_struct*)TASK_STRUCT_BASE;
    MiniUART::Init();
    // io() << "Kernel started!!\r\n";
    // io() << "test 1\r\n";
    allocator.Init();
    // io() << "Allocator started!!\r\n";
    // memset(tasks, 0, sizeof(tasks[0]));
    tasks[0].program_alloc = malloc(4096);
    // memset(tasks[0].program_alloc, 0, 4096);
    tasks[0].stack_alloc = malloc(4096);
    // memset(tasks[0].stack_alloc, 0, 4096);
    tasks[0].kernel_stack_alloc = malloc(4096);
    // memset(tasks[0].kernel_stack_alloc, 0, 4096);
    tasks[0].pid = ++pid_counter;
    total_threads++;
    sys_exec("terminal", tmp);
}
