
#include "types.h"
#include "thread.h"
#include "memory.h"
#include "mini_uart.h"
#include "cpio.h"
#include "string.h"

uint64_t total_threads = 0;

#define BEGIN_SYS uint64_t lr; asm("mov %x0, lr":"=r"(lr))
#define END_SYS asm("mov lr, %x0\neret"::"r"(lr))
#define END_SYS_RET(x) asm("mov lr, %x0\n mov x0, %x1\n eret"::"r"(lr), "r"(x))

extern "C" {
    void do_exit();
    void* memcpy(void* dst, void* src, uint64_t n);
}


static void sys_schedule() {
    uint64_t current, target;
    asm(R"(
        mrs %x0, tpidr_el1
    )":"=r"(current));
    
    target = (current + 1 < total_threads) ? current + 1 : 0;

    asm(R"(
        stp x19, x20, [%x0, 16 * 0]
        stp x21, x22, [%x0, 16 * 1]
        stp x23, x24, [%x0, 16 * 2]
        stp x25, x26, [%x0, 16 * 3]
        stp x27, x28, [%x0, 16 * 4]
        stp fp, lr, [%x0, 16 * 5]
        mrs x9, sp_el0
        str x9, [%x0, 8 * 12]
        mrs x9, elr_el1
        str x9, [%x0, 8 * 13]

        ldp x19, x20, [%x1, 16 * 0]
        ldp x21, x22, [%x1, 16 * 1]
        ldp x23, x24, [%x1, 16 * 2]
        ldp x25, x26, [%x1, 16 * 3]
        ldp x27, x28, [%x1, 16 * 4]
        ldp fp, lr, [%x1, 16 * 5]
        ldr x9, [%x1, 8 * 12]
        msr sp_el0, x9
        ldr x9, [%x1, 8 * 13]
        msr elr_el1, x9
        msr tpidr_el1, %x2
        eret
    )"::"r"(&tasks[current]), "r"(&tasks[target]), "r"(target));
}

static void sys_clone(void(*func)()) {
    uint64_t current, target;
    asm(R"(
        mrs %x0, tpidr_el1
    )":"=r"(current));
    asm(R"(
        stp x19, x20, [%x0, 16 * 0]
        stp x21, x22, [%x0, 16 * 1]
        stp x23, x24, [%x0, 16 * 2]
        stp x25, x26, [%x0, 16 * 3]
        stp x27, x28, [%x0, 16 * 4]
        stp fp, lr, [%x0, 16 * 5]
        mrs x9, sp_el0
        str x9, [%x0, 8 * 12]
        mrs x9, elr_el1
        str x9, [%x0, 8 * 13]
    )"::"r"(&tasks[current]));

    total_threads++;

    target = total_threads - 1;

    tasks[target].stack_alloc = malloc(4096);
    tasks[target].program_alloc = tasks[current].program_alloc;

    asm(R"(
        msr tpidr_el1, %x0
        msr elr_el1, %x1
        mov lr, %x2
        mov %x0, #0x3c0
        msr spsr_el1, %x0
        msr sp_el0, %x3
        eret
    )"::"r"(target), "r"(func), "r"(do_exit), "r"(tasks[target].stack_alloc + 4096));
}


static void sys_exit() {
    uint64_t current, target;
    asm(R"(
        mrs %x0, tpidr_el1
    )":"=r"(current));
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
    total_threads--;
    if (total_threads == 0) {
        MiniUART::PutS("Quit\r\n");
        asm(R"(
            1: wfi
            b 1b
        )");
    }
    if (total_threads != current) {
        tasks[current] = tasks[total_threads];
        target = (current + 1) % total_threads;
    }
    else {
        target = 0;
    }
    asm(R"(
        ldp x19, x20, [%x0, 16 * 0]
        ldp x21, x22, [%x0, 16 * 1]
        ldp x23, x24, [%x0, 16 * 2]
        ldp x25, x26, [%x0, 16 * 3]
        ldp x27, x28, [%x0, 16 * 4]
        stp fp, lr, [%x0, 16 * 5]
        mrs x9, sp_el0
        str x9, [%x0, 8 * 12]
        mrs x9, elr_el1
        str x9, [%x0, 8 * 13]
        msr tpidr_el1, %x1
        eret
    )"::"r"(&tasks[target]), "r"(target));
}

static void sys_exec(char* name, char** argv) {
    cpio_newc_header* header = (cpio_newc_header*)0x8000000;
    CPIO cpio(header);
    while (strcmp(cpio.filename, "TRILER!!!") != 0) {
        if (strcmp(cpio.filename, name) == 0) {
            uint64_t pid;
            asm("mrs %x0, tpidr_el1":"=r"(pid));
            char* arg = (char*)malloc(4096);
            char** tmp_mem = (char**)malloc(4096);
            char* sp = (char*)tasks[pid].stack_alloc + 4096;
            char* tmp = arg;
            int argc = 0;
            while(*argv != nullptr) {
                tmp_mem[argc] = tmp;
                tmp = tmp + strcpy_size(tmp, *argv);
                argc++;
                argv++;
            }
            tmp_mem[argc + 1] = nullptr;
            tmp = (char*)(((uint64_t)tmp + 7) & ~7); // Align 8
            memcpy(tmp, tmp_mem, (argc + 1) * sizeof(char*));
            free(tmp_mem);
            uint64_t arg_size = tmp - arg + (argc + 1) * sizeof(char*);
            sp -= arg_size;
            memcpy(sp, arg, arg_size);
            free(arg);
            void* program = tasks[pid].program_alloc;
            bool has_same_program = false;
            for (int i = 0; i < total_threads; i++) {
                if (i != pid && tasks[i].program_alloc == tasks[pid].program_alloc) {
                    has_same_program = true;
                    break;
                }
            }
            if (has_same_program) {
                program = malloc(4096);
            }
            memcpy(program, cpio.filecontent, cpio.filesize);
            asm(R"(
                msr elr_el1, %x2
                mov %x2, #0x3c0
                msr spsr_el1, %x2
                mov lr, %x3
                msr sp_el0, %x4
                eret
            )"::"r"(argc), "r"(tmp - arg + sp), "r"(program), "r"(do_exit), "r"(sp));
        }
        else {
            cpio = CPIO(cpio.next());
        }
    }
}

static void sys_putuart(char* str, size_t count) {
    BEGIN_SYS;
    MiniUART::PutS(str, count);
    END_SYS_RET(count);
}

static void sys_getpid() {
    BEGIN_SYS;
    uint64_t pid;
    asm ("mrs %x0, tpidr_el1":"=r"(pid));
    END_SYS_RET(pid);
}

static void sys_getuart(char* str, size_t count) {
    BEGIN_SYS;
    count = MiniUART::GetS(str, count);
    END_SYS_RET(count);
}

typedef void(* syscall_fun)();

void (*syscall_table[])() = {
    syscall_fun(sys_schedule),
    syscall_fun(sys_clone),
    syscall_fun(sys_exec),
    syscall_fun(sys_exit),
    syscall_fun(sys_putuart),
    syscall_fun(sys_getpid),
    syscall_fun(sys_getuart)
};

