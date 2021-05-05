#ifndef THREAD_H
#define THREAD_H

#include "types.h"

#define tasks (reinterpret_cast<task_struct*>(0x60000))

struct task_struct {
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
    uint64_t lr;
    uint64_t sp_el0;
    uint64_t elr_el1;
    void* stack_alloc;
    void* program_alloc;
};

static_assert(sizeof(task_struct) == 16 * 8, "task_struct is not valid");

#endif
