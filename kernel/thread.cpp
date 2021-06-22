#include <kernel/thread.h>
#include <kernel/memory_func.h>
#include <kernel/memory.h>

task_struct *tasks;
extern uint64_t total_threads;
extern uint64_t pid_counter;

void task_init() {
    memset(tasks, 0xcc, sizeof(tasks[0]));
    tasks[0].program_alloc = malloc(4096);
    tasks[0].stack_alloc = malloc(4096);
    tasks[0].kernel_stack_alloc = malloc(4096);
    tasks[0].fd_entries = malloc(4096);
    tasks[0].pid = ++pid_counter;
    tasks[0].first_free_fd = -1;
    tasks[0].first_untouched_fd = 0;
    total_threads++;
}
