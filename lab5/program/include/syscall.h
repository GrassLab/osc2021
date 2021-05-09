#pragma once

#include <types.h>
#include <string.h>

extern "C" {
    void schedule();
    void clone(void(*)());
    int exec(const char* name, const char **argv);
    void exit();
    int uart_put(const char* str, size_t count);
    int uart_get(const char* str, size_t count);
    uint64_t getpid();
    uint64_t fork();
    void delay(uint64_t cycles);
    void wait(uint64_t pid);
}
