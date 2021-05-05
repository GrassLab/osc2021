#include "mini_uart.h"
#include "memory.h"
#include "thread.h"

extern "C"
void (*syscall_table[])();

extern MemAlloc allocator;

extern"C"
void kmain() {
    char* tmp[] = {
        "hello", "hi", nullptr
    };
    allocator.Init();
    MiniUART::Init();
    tasks[0].program_alloc = malloc(4096);
    tasks[0].stack_alloc = malloc(4096);
    ((void(**)(char*, char**))syscall_table)[2]("hello", tmp);
}
