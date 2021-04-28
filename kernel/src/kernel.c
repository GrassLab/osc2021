#include "shell.h"
#include "pf_alloc.h"
#include "thread.h"
#include "scheduler.h"
#include "kernel.h"
#include "base_ops.h"

#include "io.h"

void idle()
{
    while ( 1 ) {
        printf("idle\n");
        sys_schedule();
    }
}

void foo(){
    for(int i = 0; i < 10; ++i) {
        struct Thread *current_thread = get_current_thread();
        printf("Thread id: %d %d\n", current_thread->tid, i);
        wait(100000000);
        sys_schedule();
    }
}


void kernel_main()
{
    // initialization
    init_page_frame();
    init_thread_pool();

    for(int i = 0; i < 10; ++i) { // N should > 2
        create_thread(foo);
    }

    // create shell and idle threads
    // create_thread(shell);
    create_thread(idle);

    // default pseudo thread to set first tpidr
    struct Thread t;
    t.tid = 0;
    t.pid = 0;
    t.kernel_sp = KERNEL_STACK_TOP;

    set_current_thread(&t);

    // start scheduling
    sys_schedule();
}