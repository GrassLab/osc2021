#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"
#include "string.h"

extern "C" {
    void schedule();
    void clone(void(*)());
    void exec(const char* name, const char **argv);
    void exit();
    void uart_put(const char* str, size_t count);
    void uart_get(const char* str, size_t count);
    uint64_t getpid();
    uint64_t fork();
    void delay(uint64_t cycles);
}

static inline void uart_put_1(const char *str) { uart_put(str, strlen(str)); }

#endif
