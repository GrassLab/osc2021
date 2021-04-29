#include "thread.h"
#include "../lib/uart.h"

#define THREAD_MAX 100

struct Thread thread_table[THREAD_MAX];
struct RunQueue run_queue;

void thread_init()
{
    run_queue.begin = NULL;
    run_queue.end = NULL;

    for (int i = 0; i < THREAD_MAX; ++i)
    {
        thread_table[i].id = i;
        thread_table[i].used = false;
        thread_table[i].state = Thread_Wait;
        thread_table[i].next = NULL;
    }
}

struct Thread * thread_create(void * fun_ptr)
{
    for (int i = 0; i < THREAD_MAX; ++i)
    {
        if (thread_table[i].used == false)
        {
            struct Thread * current = &thread_table[i];

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

void schedule()
{
    struct Thread * current = current_thread();
    struct Thread * next = current->next;
    //uart_puts("--schedule\n");

    if (current == run_queue.end) next = run_queue.begin;

    while (next->state != Thread_Wait)
    {
        if (next == run_queue.end)
        {
            next = run_queue.begin;
        }

        if (current == next)
        {
            return;
        }
        else
        {
            next = next->next;
        }
    }
    
    //uart_puts("--switch to\n");
    
    switch_to(current, next);
}

void kill_zombies()
{
    struct Thread * current = run_queue.begin;

    //uart_puts("---kill\n");
    while(current->next != NULL)
    {
        if (current->next->state == Thread_Exit)
        {
            struct Thread * temp = current->next->next;
            
            if (current->next == run_queue.end)
            {
                run_queue.end = current;
            }

            current->next->used = false;
            current->next->state = Thread_Wait;
            
            current->next = temp;
        }

        current = current->next;
    }
}

void idle()
{
    while(1)
    {
        //uart_puts("---Idle\n");

        kill_zombies();
        schedule();
    }
}

void exit()
{
    struct Thread * current = current_thread();

    uart_puts("---Exit\n");

    current->state = Thread_Exit;

    schedule();
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

void foo1()
{
    for (int i = 0; i < 5; ++i)
    {
        uart_puts("Thread id: ");
        uart_puts_i(current_thread()->id);
        uart_puts(" i: ");
        uart_puts_i(i);
        uart_puts("\n");
        schedule();
    }
    exit();
}

void foo2()
{

}

void thread_test(int test_id)
{
    struct Thread * default_thread = thread_create(0);
    asm volatile("msr tpidr_el1, %0"::"r"(default_thread));

    switch(test_id)
    {
        case 1:

        for (int i = 0; i < 3; ++i)
        {
            thread_create(foo1);
        }
        idle();
        break;

        case 2:
        break;
        default:
        break;
    }
}