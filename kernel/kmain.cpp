#include <kernel/mini_uart.h>
#include <kernel/memory.h>
#include <kernel/thread.h>

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
    allocator.Init();
    memset(tasks, 0xcc, sizeof(tasks[0]));
    tasks[0].program_alloc = malloc(4096);
    tasks[0].stack_alloc = malloc(4096);
    tasks[0].kernel_stack_alloc = malloc(4096);
    tasks[0].fd_entries = malloc(4096);
    tasks[0].pid = ++pid_counter;
    tasks[0].first_free_fd = -1;
    tasks[0].first_untouched_fd = 0;
    total_threads++;
    sys_exec("terminal", tmp);
}
