#include "shell.h"
#include "pf_alloc.h"
#include "thread.h"
#include "system_call.h"
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

void kernel_main()
{
    // initialization
    init_page_frame();
    init_thread_pool();

    // create shell and idle threads
    create_thread(shell);
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