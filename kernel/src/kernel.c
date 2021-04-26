#include "shell.h"
#include "pf_alloc.h"
#include "thread.h"
#include "scheduler.h"

#include "io.h"

void idle()
{
    printf("idle\n");
    // while ( 1 ) {
        // schedule();
    // }
}

void kernel_main()
{
    // initialization
    init_page_frame();
    init_thread_pool();

    // create shell and idle threads
    create_thread(shell);
    create_thread(idle);

    // start scheduling
    // schedule();
}