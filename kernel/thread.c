#include "thread.h"
#include "allocator.h"
#include "reader.h"
#include "process.h"
#include "../lib/uart.h"

#define THREAD_MAX 5

struct Thread * thread_table[THREAD_MAX];
struct RunQueue run_queue;

void thread_init()
{    
    run_queue.begin = NULL;
    run_queue.end = NULL;

    for (int i = 0; i < THREAD_MAX; ++i)
    {
        thread_table[i] = (struct Thread *)buddy_alloc(THREAD_SIZE);

        thread_table[i]->id = i;
        thread_table[i]->used = false;
        //uart_puts_h(&thread_table[i]->state);
        uart_puts("\n");
        thread_table[i]->state = Thread_Wait;
        thread_table[i]->next = NULL;

        init_fd_table(&(thread_table[i]->fd_table));
    }
}

struct Thread * thread_create(void * fun_ptr)
{
    for (int i = 0; i < THREAD_MAX; ++i)
    {
        if (thread_table[i]->used == false)
        {
            struct Thread * current = thread_table[i];

            current->used = true;
            current->context.lr = (unsigned long)fun_ptr;
            current->context.fp = (unsigned long)current + THREAD_SIZE;
            current->context.sp = (unsigned long)current + THREAD_SIZE;

            // begin -> ... -> end -> NULL
            if (run_queue.begin == NULL)
            {
                run_queue.begin = current;
                run_queue.end = current;
            }
            else
            {
                run_queue.end->next = current;
                run_queue.end = current;
            }

            return current;
        }
    }
    
    uart_puts("Error: Thread Table is full!\n");
    return NULL;
}

struct Thread * current_thread()
{
    struct Thread * current;

    asm volatile("mrs %0, tpidr_el1":"=r"(current));
    
    return current;
}

void switch_to()
{
    asm volatile("  stp x19, x20, [x0, 16 * 0]");
    asm volatile("  stp x21, x22, [x0, 16 * 1]");
    asm volatile("  stp x23, x24, [x0, 16 * 2]");
    asm volatile("  stp x25, x26, [x0, 16 * 3]");
    asm volatile("  stp x27, x28, [x0, 16 * 4]");
    asm volatile("  stp fp, lr, [x0, 16 * 5]");
    asm volatile("  mov x9, sp");
    asm volatile("  str x9, [x0, 16 * 6]");

    asm volatile("  ldp x19, x20, [x1, 16 * 0]");
    asm volatile("  ldp x21, x22, [x1, 16 * 1]");
    asm volatile("  ldp x23, x24, [x1, 16 * 2]");
    asm volatile("  ldp x25, x26, [x1, 16 * 3]");
    asm volatile("  ldp x27, x28, [x1, 16 * 4]");
    asm volatile("  ldp fp, lr, [x1, 16 * 5]");
    asm volatile("  ldr x9, [x1, 16 * 6]");
    asm volatile("  mov sp, x9");
    asm volatile("  msr tpidr_el1, x1");

    asm volatile("  ret");
}

void schedule()
{
    struct Thread * current = current_thread();
    struct Thread * next = current->next;

    // return to idle thread
    if (run_queue.begin == run_queue.end) return;
    if (current == run_queue.end) next = run_queue.begin;

    while (next->state != Thread_Wait)
    {
        if (next == run_queue.end)
        {
            next = run_queue.begin;
        }

        // already go through the whole queue
        if (current == next)
        {
            return;
        }
        else
        {
            next = next->next;
        }
    }
    
    if (next->state != Thread_Wait) return;
    /*
    uart_puts_h(current->context.lr);
    uart_puts(" switch to ");
    uart_puts_h(next->context.lr);
    uart_puts(" ");
    uart_puts_i(next->id);
    uart_puts("\n");
    */
    switch_to(&current->context, &next->context);
}

void kill_zombies()
{
    struct Thread * current = run_queue.begin;

    while(current != run_queue.end)
    {
        if (current->next->state == Thread_Exit)
        {
            current->next->used = false;
            uart_puts(" ");
            current->next->state = Thread_Wait;

            if (current->next == run_queue.end)
            {
                run_queue.end = current;
                current->next = NULL;
            }
            else
            {
                current->next = current->next->next;
            }
        }
        else
        {
            current = current->next;
        }
    }
}

void idle()
{
    while(1)
    {
        kill_zombies();
        schedule();
    }
}

void thread_exit()
{
    struct Thread * current = current_thread();

    current->state = Thread_Exit;

    schedule();
}

void thread_fork(struct Trap_Frame * tf)
{
    struct Thread * parent = current_thread();
    struct Thread * child = thread_create(0);
    int child_id = child->id;

    // task copy
    char * task_p_addr = (char*)parent;
    char * task_c_addr = (char*)child;
    int count = 0;
    while(count < THREAD_SIZE)
    {
        *(task_c_addr + count) = *(task_p_addr + count);
        count++;
    }
    
    parent->context.sp = (unsigned long)tf;
    child->id = child_id;
    
    // child copy
    int parent_ustack_size = parent->program_addr + PROCESS_SIZE - tf->sp_el0; 
    
    child->context.sp = parent->context.sp + ((unsigned long)child - (unsigned long)parent);
    child->context.fp = (unsigned long)(child + THREAD_SIZE);
    child->context.lr = (unsigned long)child_return_from_fork;

    unsigned long child_ustack = (unsigned long)buddy_alloc(PROCESS_SIZE) + PROCESS_SIZE;

    char * parent_stack_addr = (char*)(tf->sp_el0);
    char * child_stack_addr = (char*)(child_ustack - parent_ustack_size);
    child->program_addr = child_ustack - PROCESS_SIZE;

    count = 0;
    while (count < parent_ustack_size)
    {
        *(child_stack_addr + count) = *(parent_stack_addr + count);
        count++;
    }

    // return value
    struct Trap_Frame * child_tf = (struct Trap_Frame *)child->context.sp;
    child_tf->sp_el0 = child_ustack - parent_ustack_size;
    child_tf->regs[0] = 0;  // fork child return
    child_tf->regs[29] = child_ustack; // fp

    tf->regs[0] = child_id;
}

void log_runqueue()
{
    struct Thread * current = run_queue.begin;

    uart_puts("thread: ");
    
    while(current != NULL)
    {
        uart_puts_i(current->id);
        uart_puts(" -> ");

        current = current->next;
    }

    uart_puts("NULL\n");
}

/*------------------- Test -------------------*/

void foo()
{
    for (int i = 0; i < 3; ++i)
    {
        uart_puts("Thread id: ");
        uart_puts_i(current_thread()->id);
        uart_puts(" i: ");
        uart_puts_i(i);
        uart_puts("\n");
        schedule();
    }
    thread_exit();
}

void user_test()
{
    char * argv[4];
    argv[0] = "argv_test";
    argv[1] = "-o";
    argv[2] = "arg2";
    argv[3] = 0;
    do_exec("argv_test.img", argv);
}

void thread_test(int test_id)
{
    // current default thread
    struct Thread * default_thread = thread_create(0); 
    // push temp thread to tpidr, it will save by the firset context switch
    asm volatile("msr tpidr_el1, %0"::"r"(default_thread)); 

    switch(test_id)
    {
        case 1:
        for (int i = 0; i < 3; ++i)
        {
            thread_create(foo);
        }
        idle();
        break;

        case 2:
        thread_create(user_test);
        idle();
        break;

        default:
        break;
    }

    return ;
}