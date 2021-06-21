#include <kernel/mini_uart.h>
#include <kernel/memory.h>
#include <kernel/thread.h>
#include <kernel/cpio.h>
#include <kernel/string.h>
#include <kernel/fs.h>
#include <kernel/sdhost.h>
#include <kernel/fat32.h>

void sys_exec(char* name, char** argv);
extern uint64_t total_threads;
extern uint64_t pid_counter;
extern MemAlloc allocator;
task_struct *tasks;

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
    memset(tasks, 0xcc, sizeof(tasks[0]));
    tasks[0].program_alloc = malloc(4096);
    tasks[0].stack_alloc = malloc(4096);
    tasks[0].kernel_stack_alloc = malloc(4096);
    tasks[0].fd_entries = malloc(4096);
    tasks[0].pid = ++pid_counter;
    tasks[0].first_free_fd = -1;
    tasks[0].first_untouched_fd = 0;
    total_threads++;
    
    // cpio_newc_header* header = (cpio_newc_header*) INITRAMFS_BASE;
    // CPIO cpio(header);
    // while (strcmp(cpio.filename, "TRAILER!!!") != 0) {
    //     int fd = open(cpio.filename, O_WRONLY | O_CREAT);
    //     write(fd, cpio.filecontent, cpio.filesize);
    //     close(fd);
    //     cpio = CPIO(cpio.next);
    // }
    sys_exec("terminal", tmp);
}
