
#include <types.h>
#include <kernel/thread.h>
#include <kernel/memory.h>
#include <kernel/mini_uart.h>
#include <kernel/cpio.h>
#include <kernel/string.h>
#include <kernel/memory_addr.h>
#include <kernel/fs.h>

uint64_t total_threads = 0;
uint64_t pid_counter = 0;

static void schedule_internal(bool save_current) {
    uint64_t current = get_tpidr_el1();
    uint64_t target = current;
    while (true) {
        target = (target + 1 < total_threads) ? target + 1 : 0;
        if (get_timer() >= tasks[target].sleep_until) {
            if (tasks[target].wait_pid == 0) {
                break;
            }
            bool wait = false;
            for (int i = 0; i < total_threads; i++) {
                if (tasks[target].wait_pid == tasks[i].pid) {
                    wait = true;
                    break;
                }
            }
            if (!wait) {
                tasks[target].wait_pid = 0;
                break;
            }
        }
    }
    switch_to(save_current ? &tasks[current].regs : nullptr, &tasks[target].regs, tasks[target].page_table, target);
}

static void sys_schedule() {
    schedule_internal(true);
}

static void sys_clone(void *func) {
    uint64_t current = get_tpidr_el1();
    uint64_t target = total_threads;
    total_threads++;
    memcpy(&tasks[target], &tasks[current], sizeof(decltype(*tasks)));
    tasks[target].pid = ++pid_counter;
    tasks[target].regs.lr = (void*)kernel_thread_start;
    tasks[target].regs.elr_el1 = func;
    tasks[target].stack_alloc = malloc(4096);
    tasks[target].kernel_stack_alloc = malloc(4096);
    tasks[target].regs.sp_el0 = tasks[target].stack_alloc + 4096;
    tasks[target].regs.sp = tasks[target].kernel_stack_alloc + 4096;
    tasks[target].sleep_until = 0;
}


void sys_exit() {
    uint64_t current = get_tpidr_el1();
    bool has_same_program = false;
    for (int i = 0; i < total_threads; i++) {
        if (i != current && tasks[i].program_alloc == tasks[current].program_alloc) {
            has_same_program = true;
            break;
        }
    }
    if (!has_same_program) {
        free(tasks[current].program_alloc);
    }
    free(tasks[current].stack_alloc);
    free(tasks[current].kernel_stack_alloc);
    free(tasks[current].fd_entries);
    total_threads--;
    if (total_threads == 0) {
        MiniUART::PutS("Kernel exit!! Shutting down\r\n");
        qemu_quit();
    }
    if (total_threads != current) {
        tasks[current] = tasks[total_threads];
    }
    schedule_internal(false);
}

int sys_exec(char* name, char** argv) {
    char buffer[25];
    cpio_newc_header* header = (cpio_newc_header*) INITRAMFS_BASE;
    CPIO cpio(header);
    while (strcmp(cpio.filename, "TRAILER!!!") != 0) {
        if (strcmp(cpio.filename, name) == 0) {
            uint64_t tpidr = get_tpidr_el1();
            char* arg = (char*)malloc(4096);
            char** tmp_mem = (char**)malloc(4096);
            char* sp_el0 = (char*)tasks[tpidr].stack_alloc + 4096;
            char* tmp = arg;
            int argc = 0;
            while(*argv != nullptr) {
                tmp_mem[argc] = tmp;
                tmp = tmp + strcpy_size(tmp, *argv);
                argc++;
                argv++;
            }
            tmp_mem[argc + 1] = nullptr;
            tmp = (char*)(((uint64_t)tmp + 15) & ~15); // Align 16
            memcpy(tmp, tmp_mem, (argc + 1) * sizeof(char*));
            free(tmp_mem);
            uint64_t arg_size = tmp - arg + (argc + 1) * sizeof(char*);
            arg_size = (arg_size + 15) & ~15; // Align 16
            sp_el0 -= arg_size;
            memcpy(sp_el0, arg, arg_size);
            free(arg);
            char *arg_final = sp_el0 + (tmp - arg);
            for (int i = 0; i < argc; i++) {
                ((char**)arg_final)[i] -= (arg - ((char*)tasks[tpidr].stack_alloc) - 0x1000 + arg_size);
                ((char**)arg_final)[i] = (char*)((uint64_t(((char**)arg_final)[i]) & 0xfff) | 0x10000000);
            }
            bool has_same_program = false;
            for (int i = 0; i < total_threads; i++) {
                if (i != tpidr && tasks[i].program_alloc == tasks[tpidr].program_alloc) {
                    has_same_program = true;
                    break;
                }
            }
            if (!has_same_program) {
                free(tasks[tpidr].program_alloc);
            }
            tasks[tpidr].program_size = (cpio.filesize + 4095) & ~4095; // Align 4096
            tasks[tpidr].regs.sp = tasks[tpidr].kernel_stack_alloc + 4096;
            tasks[tpidr].regs.lr = (void*)kernel_thread_start;
            tasks[tpidr].program_alloc = memcpy(malloc(tasks[tpidr].program_size), cpio.filecontent, cpio.filesize);
            tasks[tpidr].page_table->program_alloc = uint64_t(kernel_to_physical(tasks[tpidr].program_alloc)) | USER_FLAG;
            tasks[tpidr].regs.elr_el1 = (void *)0x10001000;
            tasks[tpidr].regs.sp_el0 = (void*)((uint64_t(sp_el0) & 0xfff) | 0x10000000);
            tasks[tpidr].sleep_until = 0;
            tasks[tpidr].wait_pid = 0;
            switch_to(nullptr, &tasks[tpidr].regs, tasks[tpidr].page_table, tpidr, argc, (uint64_t(arg_final) & 0xfff) | 0x10000000);
        }
        else {
            cpio = CPIO(cpio.next);
        }
    }
    return -1;
}

static size_t sys_putuart(char* str, size_t count) {
    MiniUART::PutS(str, count);
    return count;
}

static uint64_t sys_getpid() {
    return tasks[get_tpidr_el1()].pid;
}

static size_t sys_getuart(char* str, size_t count) {
    return MiniUART::GetS(str, count);
}

static uint64_t sys_fork() {
    uint64_t parent = get_tpidr_el1();
    uint64_t parent_pid = tasks[parent].pid;
    uint64_t child = total_threads++;
    uint64_t child_pid = ++pid_counter;
    memcpy(&tasks[child], &tasks[parent], sizeof(tasks[0]));
    tasks[child].pid = child_pid;
    tasks[child].stack_alloc = malloc(4096);
    tasks[child].kernel_stack_alloc = malloc(4096);
    tasks[child].fd_entries = malloc(4096);
    memcpy(tasks[child].fd_entries, tasks[parent].fd_entries, 4096);
    tasks[child].program_alloc = tasks[parent].program_alloc;
    tasks[child].page_table = (struct page_table_struct*)malloc(4096);
    tasks[child].page_table->program_alloc = uint64_t(kernel_to_physical(tasks[child].program_alloc)) | USER_FLAG;
    tasks[child].page_table->stack_alloc = uint64_t(kernel_to_physical(tasks[child].stack_alloc)) | USER_FLAG | NON_EXECUTE_FLAG;
    tasks[child].sleep_until = 0;
    fork_internal(&tasks[parent], &tasks[child], tasks[parent].stack_alloc, tasks[child].stack_alloc, tasks[parent].kernel_stack_alloc, tasks[child].kernel_stack_alloc);
    if (tasks[get_tpidr_el1()].pid != parent_pid) {
        return 0;
    }
    return child_pid;
}

static void sys_delay(uint64_t cycles) {
    uint64_t current_time = get_timer();
    uint64_t current = get_tpidr_el1();
    tasks[current].sleep_until = current_time + cycles;
    sys_schedule();
}

static void sys_wait(uint64_t pid) {
    uint64_t current = get_tpidr_el1();
    tasks[current].wait_pid = pid;
    sys_schedule();
}

typedef void(* syscall_fun)();

void (*syscall_table[])() = {
    syscall_fun(sys_schedule),
    syscall_fun(sys_clone),
    syscall_fun(sys_exec),
    syscall_fun(sys_exit),
    syscall_fun(sys_putuart),
    syscall_fun(sys_getpid),
    syscall_fun(sys_getuart),
    syscall_fun(sys_fork),
    syscall_fun(sys_delay),
    syscall_fun(sys_wait),
    syscall_fun(open),
    syscall_fun(read),
    syscall_fun(write),
    syscall_fun(close),
};

