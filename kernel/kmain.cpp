#include <kernel/mini_uart.h>
#include <kernel/memory.h>
#include <kernel/thread.h>
#include <kernel/cpio.h>
#include <kernel/string.h>
#include <kernel/fs.h>
#include <kernel/sdhost.h>
#include <kernel/fat32.h>

void sys_exec(char* name, char** argv);
extern MemAlloc allocator;

char buffer[512];

extern"C"
void kmain() {
    char* tmp[] = {
        "terminal", nullptr
    };
    tasks = (task_struct*)TASK_STRUCT_BASE;
    MiniUART::Init();
    allocator.Init();
    sd_init();
    fs_init();
    task_init();
    
    sys_exec("terminal", tmp);
}
