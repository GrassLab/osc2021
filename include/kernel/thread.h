#pragma once

#include <types.h>

struct task_reg_struct {
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t fp;
    void *lr;
    void *sp_el0;
    void *elr_el1;
    void *sp;
};

struct page_table_struct {
    uint64_t stack_alloc;
    uint64_t program_alloc;
};

struct alignas(16) task_struct {
    task_reg_struct regs;
    void *stack_alloc;
    void *kernel_stack_alloc;
    void *program_alloc;
    void *fd_entries;
    page_table_struct *page_table;
    uint64_t pid;
    uint64_t sleep_until;
    uint64_t program_size;
    uint64_t wait_pid;
    int64_t first_free_fd;
    int64_t first_untouched_fd;
};

#define NON_EXECUTE_FLAG 0x40000000000000
#define USER_FLAG        0x447

extern task_struct* tasks;

inline void* kernel_to_physical(void *addr) {
    return (void*)(uint64_t(addr) & 0xffffffffffff);
}

void task_init();

extern "C" {
    void do_exit();
    void switch_to(task_reg_struct *from, task_reg_struct *to, page_table_struct *page, uint64_t to_tpidr, ...);
    uint64_t get_tpidr_el1();
    void loop();
    void set_eret_addr(void *addr);
    void kernel_thread_start();
    uint64_t fork_internal(task_struct *parent, task_struct *child, void *parent_stack, void *child_stack, void *parent_kernel_stack, void *child_kernel_stack);
    void qemu_quit();
    uint64_t get_timer();
}
