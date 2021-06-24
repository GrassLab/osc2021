#include <kernel/thread.h>
#include <kernel/memory_func.h>
#include <kernel/memory.h>
#include <kernel/memory_addr.h>

task_struct *tasks;
extern uint64_t total_threads;
extern uint64_t pid_counter;

void task_init() {
    memset(tasks, 0xcc, sizeof(tasks[0]));
    tasks[0].kernel_stack_alloc = malloc(4096);
    tasks[0].fd_entries = malloc(4096);
    tasks[0].program_alloc = malloc(4096);
    tasks[0].stack_alloc = malloc(4096);
    tasks[0].pid = ++pid_counter;
    tasks[0].first_free_fd = -1;
    tasks[0].first_untouched_fd = 0;
    tasks[0].page_table = (struct page_table_struct*)malloc(4096);
    memset(tasks[0].page_table, 0, 4096);
    *(uint64_t*)PAGE_TABLE_ADDR = (uint64_t(tasks[0].page_table) & 0x0000ffffffffffff) | 3;
    tasks[0].page_table->program_alloc = uint64_t(kernel_to_physical(tasks[0].program_alloc)) | USER_FLAG;
    tasks[0].page_table->stack_alloc = uint64_t(kernel_to_physical(tasks[0].stack_alloc)) | USER_FLAG | NON_EXECUTE_FLAG;
    total_threads++;
}
