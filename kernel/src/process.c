#include "thread.h"
#include "def.h"
#include "pf_alloc.h"
#include "dynamic_alloc.h"

#include "io.h"

int distribute_pid = 0;

int get_new_pid()
{
    int tmp = distribute_pid;
    distribute_pid++;

    return tmp;
}

struct Thread * create_process(void *source_addr, int size)
{
    char *content_addr = (char *)source_addr;
    void *start_addr = NULL;
    
    /* important!!! 
     * This address is tightly bound with the linker script 
     * of that program when creation, in order to clear bss at the right address
     * (fork is okay since bss is already clean)
    */
    alloc_page(&start_addr, 17); // FIXIT: hardcoded

    void *sp_addr = (void *)((uint64_t)start_addr + (2^17));
    
    char *dest_tmp = (char *)start_addr;

    // copy context from source
    for (int i = 0; i < size; i++, content_addr++, dest_tmp++) {
        *dest_tmp = *content_addr;
    }

    // create process' default thread
    struct Thread *t = malloc(sizeof(struct Thread));
    printf("here");

    t->pid = get_new_pid(); // since this is a process
    t->tid = get_new_tid();
    t->state = RUNNING;

    t->code = start_addr;

    t->user_sp = t->fp = (uint64_t)sp_addr; // 256 for user sp
    t->kernel_sp = (uint64_t)sp_addr + 256; // 256 for kernel sp

    thread_pool_add(t);
    return t;
}