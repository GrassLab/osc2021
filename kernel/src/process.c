#include "thread.h"
#include "def.h"
#include "pf_alloc.h"
#include "dynamic_alloc.h"

#include "math.h"
#include "io.h"
#include "cpio.h"
#include "scheduler.h"
#include "string.h"

int distribute_pid = 1;

int get_new_pid()
{
    int tmp = distribute_pid;
    distribute_pid++;

    return tmp;
}

struct Thread * create_process(void *source_addr, int size, int argc, char *argv[])
{
    char *content_addr = (char *)source_addr;
    void *start_addr = NULL;
    
    /* important!!! 
     * This address is tightly bound with the linker script 
     * of that program when creation, in order to clear bss at the right address
     * (fork is okay since bss is already clean)
    */
    alloc_page(&start_addr, 17); // FIXIT: hardcoded
    
    char *dest_tmp = (char *)start_addr;

    // copy context from source
    for (int i = 0; i < size; i++, content_addr++, dest_tmp++) {
        *dest_tmp = *content_addr;
    }

    // create process' default thread
    struct Thread *t = malloc(sizeof(struct Thread));

    t->pid = get_new_pid(); // since this is a process
    t->tid = get_new_tid();
    t->state = RUNNING;
    t->code = start_addr;

    // // create thread stack
    alloc_page((void **)&t->kernel_sp, THREAD_STACK_SIZE);
    t->user_sp = t->kernel_sp - int_pow(2, THREAD_STACK_SIZE - 1); // half for user sp

    t->kernel_sp -= 256;
    struct context *ctx = (struct context *)t->kernel_sp;
    ctx->lr = (uint64_t)start_addr;
    ctx->fp = t->user_sp;

    uint64_t tmp_user_sp = t->user_sp;
    for (int i = 0; i < argc; i++) {
        printf("copying \"%s\"...\n", argv[i]);
        t->user_sp -= (strlen(argv[i]) + 1);
        strcpy((char *)t->user_sp, argv[i]);
        printf("copyied \"%s\"...\n", (char *)t->user_sp);
    }

    t->user_sp -= (8 * argc);
    uint64_t **tmp = (uint64_t **)t->user_sp;
    for (int i = 0; i < argc; i++) {
        tmp_user_sp -= (strlen(argv[i]) + 1);
        tmp[i] = (uint64_t *)tmp_user_sp;
    }   

    ctx->reg[0] = argc;
    ctx->reg[1] = t->user_sp;
    


    



    // *((int *)t->user_sp) = argc;


    thread_pool_add(t);
    enqueue(t);
    return t;
}

void do_exec(char *name, char *argv[])
{
    // calculate argc
    int argc = 0, i = 0;
    while (argv[i] != 0) {
        argc++;
        i++;
    }

    cpio_exec(name, argc, argv);
}