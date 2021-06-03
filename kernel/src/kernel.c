#include "shell.h"
#include "pf_alloc.h"
#include "thread.h"
#include "system_call.h"
#include "kernel.h"
#include "base_ops.h"
#include "vfs.h"
// #include "tmpfs.h"
#include "fat32.h"
#include "sdhost.h"

#include "io.h"

// extern struct filesystem tmpfs;
extern struct filesystem fat32;

void idle()
{
    while ( 1 ) {
        printf("idle\n");
        sys_schedule();
        wait(1000000);
    }
}

void kernel_main()
{
    // initialization
    init_page_frame();
    init_thread_pool();
    sd_init();

    // mount root file system
    // register_filesystem(&tmpfs);
    // init_root("tmpfs");

    register_filesystem(&fat32);
    init_root("fat32");

    // default pseudo thread to set first tpidr
    struct Thread t;
    t.tid = 0;
    t.pid = 0;
    t.kernel_sp = KERNEL_STACK_TOP;

    set_current_thread(&t);
    
    // create shell and idle threads
    create_thread(shell);
    create_thread(idle);

    // start scheduling
    sys_schedule();
}