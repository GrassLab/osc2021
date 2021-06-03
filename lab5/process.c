#include "include/process.h"
#include "include/scheduler.h"
#include "include/allocator.h"
#include "include/uart.h"
#include "include/util.h"
#include "include/interrupt.h"
#include "include/cpio.h"

static int pids[MAX_PID_NUM] = {0};

int new_pid() {
    static int last_pid = 0;
    int search_times = 0;
    while(pids[last_pid++]) {
        last_pid++;
        last_pid %= MAX_PID_NUM;
        if (search_times++ > MAX_PID_NUM) {
            uart_put_str("pid is not enough.\n");
            return -1;
        }
    }
    pids[last_pid - 1] = 1;
    return last_pid - 1;
}

void erase_pid(int pid) {
    pids[pid] = 0;
}

mm_struct *new_mm(int size, int seg_type) {
    mm_struct *mm = (mm_struct *)get_memory(sizeof(mm_struct));
    mm->address = get_memory(size);
    mm->size = size;
    mm->seg_type = seg_type;
    mm->owner_num = 1;
    mm->next = NULL;
    mm->prev = NULL;
    mm->same_mm_owner_next = mm;
    mm->same_mm_owner_prev = mm;
    return mm;
}

void add_mm(task_struct *task, mm_struct *mm) {
    mm->next = task->mm;
    if (task->mm)
        task->mm->prev = mm;
    task->mm = mm;
}

mm_struct *share_same_mm(mm_struct *exist_mm) {
    mm_struct *new;
    new = new_mm(0, exist_mm->seg_type);
    new->size = exist_mm->size;
    new->address = exist_mm->address;
    
    mm_struct *tmp_mm = exist_mm;
    new->owner_num = exist_mm->owner_num;
    new->same_mm_owner_next = exist_mm;
    new->same_mm_owner_prev = exist_mm->same_mm_owner_prev;
    new->same_mm_owner_next->same_mm_owner_prev = new;
    new->same_mm_owner_prev->same_mm_owner_next = new;
            
    for (int i = 0; i < new->owner_num; i++) {
        tmp_mm->owner_num++;
        tmp_mm = tmp_mm->same_mm_owner_next;
    }
    new->owner_num++;
    return new;
}

void copy_mm(task_struct *old, task_struct *new) {
    mm_struct *old_mm = old->mm;
    mm_struct *mm;
    while (old_mm) {
        if (old_mm->seg_type != CODE_SEG) {
            mm = new_mm(old_mm->size, old_mm->seg_type);
            char *old_addr = (char *)old_mm->address;
            char *new_addr = (char *)mm->address;
            for (int i = 0; i < old_mm->size; i++) {
                new_addr[i] = old_addr[i];
            }
            add_mm(new, mm);
        } else {
            mm = share_same_mm(old_mm);
            add_mm(new, mm);
        }
        old_mm = old_mm->next;
    }
}

unsigned long get_mm_address(mm_struct *mm, int seg_type) {
    while (mm) {
        if (mm->seg_type == seg_type)
            return mm->address;
        mm = mm->next;
    }
    return 0;
}

unsigned long get_true_address_with_fork(task_struct *src, task_struct *dest, unsigned long src_addr) {
    mm_struct *src_mm = src->mm;
    mm_struct *dest_mm = dest->mm;
    while (src_mm) {
        if (src_mm->address < src_addr && src_addr <= src_mm->address + src_mm->size) {
            while (dest_mm) {
                if (dest_mm->seg_type == src_mm->seg_type)
                    return src_addr - src_mm->address + dest_mm->address;
                dest_mm = dest_mm->next;
            }
        }
        src_mm = src_mm->next;
    }
    return 0;
}

mm_struct *search_stack(mm_struct *mm, int seg_type) {
    while (mm) {
        if (mm->seg_type == seg_type)
            return mm;
        mm = mm->next;
    }
    return mm;
}

void init_cpu_context(task_struct *task) {
    task->cpu_context.x19 = 0;
    task->cpu_context.x20 = 0;
    task->cpu_context.x21 = 0;
    task->cpu_context.x22 = 0;
    task->cpu_context.x23 = 0;
    task->cpu_context.x24 = 0;
    task->cpu_context.x25 = 0;
    task->cpu_context.x26 = 0;
    task->cpu_context.x27 = 0;
    task->cpu_context.x28 = 0;
    task->cpu_context.fp = 0;
    task->cpu_context.lr = 0;
    task->cpu_context.spsr_el1 = 0;
    task->cpu_context.elr_el1 = 0;
    task->cpu_context.sp_el0 = 0;
    task->cpu_context.sp = 0;
}

task_struct *create_task() {
    task_struct *task = (task_struct *)get_memory(sizeof(task_struct));
    init_cpu_context(task);
    task->pid = new_pid();
    task->last_time = get_time();
    task->use_time = 0;
    task->mm = NULL;
    task->state = 0;
    return task;
}

void free_task_memory(mm_struct *mm) {
    while (mm) {
        mm->owner_num--;
        mm_struct *tmp_mm = mm->same_mm_owner_next;
        for (int i = 0; i < mm->owner_num; i++) {
            tmp_mm->owner_num--;
            tmp_mm = tmp_mm->same_mm_owner_next;
        }
        mm->same_mm_owner_next->same_mm_owner_prev = mm->same_mm_owner_prev;
        mm->same_mm_owner_prev->same_mm_owner_next = mm->same_mm_owner_next;
        if (mm->owner_num == 0)
            free_memory(mm->size, mm->address);
        tmp_mm = mm;
        mm = mm->next;
        free_memory((unsigned long)sizeof(mm_struct), (unsigned long)tmp_mm);
    }
}

void init_thread() {
    task_struct *task = create_task();
    mm_struct *mm;
    mm = new_mm(0, KSTACK_SEG);
    mm->size = 0x80000;
    mm->address = 0x0;
    add_mm(task, mm);
    mm = new_mm(0, CODE_SEG);
    mm->size = 0x40000;
    mm->address = 0x80000;
    add_mm(task, mm);
    push_run_queue(task);
    asm volatile(
        "mov x10, 0x0\n\t"
        "msr spsr_el1, x10\n\t"
        "msr tpidr_el1, %[task]\n\t"::[task] "r" (task):
    );
}

void thread_create(void (*thread)()) {
    task_struct *task = create_task();
    mm_struct *mm = new_mm(FRAME_SIZE, KSTACK_SEG);
    add_mm(task, mm);

    unsigned long spsr_el1, elr_el1, sp_el0, kernel_sp;
    asm volatile (
        "mrs %[spsr_el1], spsr_el1\n\t"
        "mrs %[elr_el1], elr_el1\n\t"
        "mrs %[sp_el0], sp_el0\n\t"
        : [spsr_el1] "=r" (spsr_el1), [elr_el1] "=r" (elr_el1), [sp_el0] "=r" (sp_el0)
        :
        :
    );
    kernel_sp = get_mm_address(task->mm, KSTACK_SEG) + FRAME_SIZE;
    task->cpu_context.lr = (unsigned long)thread;
    task->cpu_context.spsr_el1 = spsr_el1;
    task->cpu_context.elr_el1 = elr_el1;
    task->cpu_context.sp_el0 = sp_el0;
    task->cpu_context.sp = kernel_sp;
    push_run_queue(task);
}

int exec(const char* name, char *const argv[]) {
    init_cpu_context(current);
    // free all current mm but kernel stack
    mm_struct *old_kstack = search_stack(current->mm, KSTACK_SEG);
    mm_struct *mm = share_same_mm(old_kstack);
    free_task_memory(current->mm);
    current->mm = NULL;
    unsigned long new_ksp = mm->address + FRAME_SIZE;
    add_mm(current, mm);
    // user stack
    mm = new_mm(FRAME_SIZE, USTACK_SEG);
    unsigned long new_usp = mm->address + FRAME_SIZE;
    add_mm(current, mm);
    // text
    unsigned long start_addr;
    int filesize;
    copy_file(name, &start_addr, &filesize);
    mm = new_mm(0, CODE_SEG);
    mm->size = filesize;
    mm->address = start_addr;
    add_mm(current, mm);
    // get trap frame size
    int argc = 0;
    int strs_size = 0;
    const char *str;
    while ((str = argv[argc++])) {
        int str_size = 0;
        while (str[str_size++]);
        strs_size += str_size;
    }
    new_usp -= align(argc * 8 + strs_size, 16);
    /*
     *                      +-------------------------------------------------------+
     *                      |                                                       ↓
     *  ◆x29(8), x30(8), argv(8), null(4), argc(4), local variable(align8(n)), ◇argc[0](8), argc[1](8), null(8), "a.out\0", "arg1\0"
     *                                                                              |             |                   ↑          ↑
     *                                                                              +-------------|-------------------+          |
     *                                                                                            +------------------------------+      
     *  ◆ is sp after process operate, ◇ is sp(new_usp) which set by exec
     */
    // set trap frame
    char *args_addr = (char *)(new_usp + argc * 8);
    for (int i = 0; i < argc - 1; i++) {
        str = argv[i];
        ((unsigned long *)new_usp)[i] = (unsigned long)args_addr;
        while (*str) {
            *args_addr++ = *str++;
        }
        *args_addr++ = '\0';
    }
    ((unsigned long *)new_usp)[argc - 1] = NULL;
    // set eret address and kernel sp
    argc--;
    current->last_time = get_time();
    current->use_time = 0;
    asm volatile (
        "mov x10, 0x0\n\t"  // mov x10, 0x3c0 (set D, A, I, F to 1)
        "msr spsr_el1, x10\n\t"
        "msr elr_el1, %[start_addr]\n\t"
        "msr sp_el0, %[new_usp]\n\t"
        "mov sp, %[ksp]\n\t"
        "mov x1, %[new_usp]\n\t"
        "mov x0, %[argc]\n\t"
        "eret\n\t"
        : 
        : [start_addr] "r" (start_addr), [ksp] "r" (new_ksp),
          [new_usp] "r" (new_usp), [argc] "r" (argc)
        :
    );
    return 0;
}

void exit() {
    pop_run_queue_by_pid(current->pid);
    push_terminal_queue(current);
    schedule();
}

int fork() {
    task_struct *new = create_task();
    copy_mm(current, new);
    new->cpu_context = current->cpu_context;
    unsigned long sp_el0, elr_el1;
    asm volatile (
        "mrs %[sp_el0], sp_el0\n\t"
        "mrs %[elr_el1], elr_el1\n\t"
        : [sp_el0] "=r" (sp_el0), [elr_el1] "=r" (elr_el1)
        :
        :
    );
    new->cpu_context.lr = (unsigned long)ret_from_fork;
    new->cpu_context.sp_el0 = get_true_address_with_fork(current, new, sp_el0);
    // 32 * 8 use for store x0-x30, read load_all in interrupt.S
    new->cpu_context.sp = search_stack(new->mm, KSTACK_SEG)->address + FRAME_SIZE - 32 * 8;
    new->cpu_context.elr_el1 = elr_el1;
    new->cpu_context.spsr_el1 = 0x0;
    push_run_queue(new);
    return new->pid;
}

void kill_zombies() {
    task_struct *task;
    while ((task = pop_terminal_queue_head())) {
        free_task_memory(task->mm);
        erase_pid(task->pid);
        free_memory((unsigned long)sizeof(task_struct), (unsigned long)task);
    }
}