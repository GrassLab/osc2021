#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

extern "C" {
    void schedule();
    void clone();
    void exec(const char* name, const char **argv);
    void exit();
    void uart_put(const char* str, size_t count);
    void uart_get(const char* str, size_t count);
    uint64_t getpid();
}

#endif
