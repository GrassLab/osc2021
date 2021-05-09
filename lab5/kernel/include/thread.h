#pragma once

#include <types.h>

struct alignas(16) task_struct {
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
    void *stack_alloc;
    void *kernel_stack_alloc;
    void *program_alloc;
    uint64_t pid;
    uint64_t sleep_until;
    uint64_t program_size;
    uint64_t wait_pid;
};

extern task_struct* tasks;
