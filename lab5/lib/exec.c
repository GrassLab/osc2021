#include <fs.h>
#include <stat.h>
#include <sched.h>
#include <mm.h>
#include <string.h>
#include <fs/cpio.h>
#include <context_switch.h>
#include <elf.h>
#include <printf.h>
#include <interrupt.h>
#include <preempt.h>

static uint32_t align_up(uint32_t size, int alignment) {
  return (size + alignment - 1) & -alignment;
}

static void *init_stack_args(char *stack, const char *argv[]) {
    const char *s;
    unsigned argc = 0;
    unsigned s_size = 0;
    unsigned a_size;
    while (s = argv[argc]) {
        /* TODO: validate pointer */
        s_size += strlen(s) + 1;
        argc++;
    }

    s_size = align_up(s_size, 16);
    /* total 1(argc) + argc+1(argv+NULL) */
    a_size = align_up((argc + 2) * sizeof(size_t), 16);
    size_t *top = (size_t *)(stack - (s_size + a_size));
    char *s_ptr = (char *)top + a_size;

    top[0] = argc;
    for (int i = 0; i < argc; i++) {
        top[i+1] = (size_t)s_ptr;
        unsigned n = strlen(argv[i]) + 1;
        memcpy(s_ptr, argv[i], n);
        s_ptr += n;
    }
    top[argc+1] = 0;

    return top;
}

/* in-place substitute all task_struct data, return to new context */
/* (is in-place substitute a bad idea ??) */
/* TODO: check if disable preempt is enough */
static void replace_user_context(void *prog, void *entry, const char *argv[]) {
    disable_preempt();

    struct task_struct *ts = current;

    /* TODO: protect page with virtual memory */
    memset(ts->stack, 0, USTACK_SIZE);
    kfree(ts->user_prog);
    ts->user_prog = prog;

    char *ustack_top = init_stack_args(ts->stack + USTACK_SIZE, argv);

    /* svc call initiate by user should only and always have one trapframe on kstack */
    /* so we won't accidentlly corrupt the stack we use now */
    struct pt_regs *trapframe = (struct pt_regs *)((char *)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs));
    memset(trapframe, 0, sizeof(struct pt_regs));

    trapframe->pc = (size_t)entry;
    trapframe->sp = (size_t)ustack_top;

    enable_preempt();
}

int do_exec(const char *path, const char *argv[]) {
    /* TODO: use function array to handle file open/read */
    struct file *f = NULL;
    void *prog = NULL;

    f = cpio_get_file(path);
    if (!f || !(S_IXUSR | f->f_mode)) {
        goto FAILED;
    }

    if (!check_elf(f->data)) {
        goto FAILED;
    }

    prog = kcalloc(f->size);
    memcpy(prog, f->data, f->size);

    void *entry = elf_entry(prog);
    if (!entry) {
        goto FAILED;
    }

    kfree(f);

    /* eret to new user context */
    replace_user_context(prog, entry, argv);
    return 0;

FAILED:
    kfree(f);
    kfree(prog);

    return -1;
}

/* only used for kthread to initialize a userland process */
static void exec_user_context(void *prog, void *entry, const char *argv[]) {
    disable_interrupt();

    struct task_struct *ts = current;

    /* TODO: protect page with virtual memory */
    ts->user_prog = prog;
    ts->stack = kmalloc(USTACK_SIZE);
    memset(ts->stack, 0, USTACK_SIZE);

    char *stack_top = init_stack_args(ts->stack + USTACK_SIZE, argv);

    struct pt_regs *trapframe = (struct pt_regs *)((char *)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs));
    memset(trapframe, 0, sizeof(struct pt_regs));

    trapframe->pc = (size_t)entry;
    trapframe->sp = (size_t)stack_top;
    trapframe->pstate = 0;

    asm(
        "mov sp, %0\n\t"
        "b restore_regs_eret"
    ::"r"(trapframe));
}

int kernel_exec_file(const char *path, const char *argv[]) {
    /* TODO: use function array to handle file open/read */
    struct file *f = NULL;
    void *prog = NULL;

    f = cpio_get_file(path);
    if (!f || !(S_IXUSR | f->f_mode)) {
        goto FAILED;
    }

    if (!check_elf(f->data)) {
        goto FAILED;
    }

    prog = kcalloc(f->size);
    memcpy(prog, f->data, f->size);

    void *entry = elf_entry(prog);
    if (!entry) {
        goto FAILED;
    }

    kfree(f);

    /* instantly eret to userland */
    exec_user_context(prog, entry, argv);

FAILED:
    kfree(f);
    kfree(prog);

    return -1;
}