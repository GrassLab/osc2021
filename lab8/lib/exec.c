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
#include <fs/vfs.h>
#include <file.h>
#include <syscall_wrapper.h>
#include <mmu.h>

static size_t align_up(size_t size, int alignment) {
  return (size + alignment - 1) & -alignment;
}

static size_t align_down(size_t size, int alignment) {
  return size & -alignment;
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

static struct vm_area *load_elf_segment(pd_t *tbl, void *elf) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf;
    Elf64_Phdr *phdr = (Elf64_Phdr *)((char *)elf + ehdr->e_phoff);
    struct vm_area *vmlist = NULL;

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            /* assuming the offset is the same */
            unsigned long va_aligned = align_down(phdr[i].p_vaddr, PAGE_SIZE);
            unsigned long va_off = phdr[i].p_vaddr & (PAGE_SIZE - 1);
            unsigned long offset_aligned = align_down(phdr[i].p_offset, PAGE_SIZE);
            unsigned long offset_off = phdr[i].p_offset & (PAGE_SIZE - 1);

            if (va_off != offset_off) {
                panic("offset is different");
            }

            /* the total of .data + .bss */
            const unsigned long total_size = align_up(phdr[i].p_memsz + va_off, PAGE_SIZE);
            const unsigned long file_size = align_up(phdr[i].p_filesz + va_off, PAGE_SIZE);

            struct vm_area *vm = kmalloc(sizeof(struct vm_area) + sizeof(size_t) * (total_size / PAGE_SIZE));
            vm->va = va_aligned;
            vm->size = total_size;
            vm->flags = phdr[i].p_flags;
            vm->next = vmlist;
            vmlist = vm;

            /* TODO: zero out offset/ending part */
            for (int j = 0; j < file_size; j += PAGE_SIZE) {
                void *p = kcalloc(PAGE_SIZE);
                vm->va_map[j / PAGE_SIZE] = p;
                map_user_page(tbl, va_aligned + j, virt_to_phys(p), phdr[i].p_flags);
                memcpy(p, elf + offset_aligned + j, PAGE_SIZE);
            }

            for (int j = file_size; j < total_size ; j += PAGE_SIZE) {
                void *p = kcalloc(PAGE_SIZE);
                vm->va_map[j / PAGE_SIZE] = p;
                map_user_page(tbl, va_aligned + j, virt_to_phys(p), phdr[i].p_flags);
            }
        }
    }

    return vmlist;
}

/* in-place substitute all task_struct data, return to new context */
/* (is in-place substitute a bad idea ??) */
/* TODO: check if disable preemption is enough */
static void replace_user_context(pd_t *tbl, struct vm_area *vmlist, size_t entry, const char *argv[]) {
    disable_preempt();

    struct task_struct *ts = current;
    kfree(ts->stack);

    /* TODO: should copy argv to kernel space */
    /* alloc before freeing page table (accessing argv cause segfault) */
    ts->stack = alloc_user_stack(tbl, USTACK_SIZE);
    char *stack_top = init_stack_args(ts->stack + USTACK_SIZE, argv);
    size_t sp_off = stack_top - ts->stack;

    /* replace page table */
    asm(
        "dsb ish\n\t"           /* ensure write has completed */
        "msr ttbr0_el1, %0\n\t" /* switch ttb */
        "tlbi vmalle1is\n\t"    /* invalidate all TLB entries */
        "dsb ish\n\t"           /* ensure completion of TLB invalidatation */
        "isb\n\t"               /* clear pipeline */
    ::"r"(tbl));

    /* free previous memory space */
    free_user_vm(ts->ttbr0, ts->vm_list);
    ts->ttbr0 = tbl;
    ts->vm_list = vmlist;

    /* svc call initiate by user should only and always have one trapframe on kstack */
    /* so we won't accidentlly corrupt the stack we use now */
    struct pt_regs *trapframe = (struct pt_regs *)((char *)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs));
    memset(trapframe, 0, sizeof(struct pt_regs));

    trapframe->pc = entry;
    trapframe->sp = USTACK_VA + sp_off;

    enable_preempt();
}

int do_exec(const char *path, const char *argv[]) {
    struct file *f;
    void *elf;

    f = vfs_open(path, 0);
    if (!f || !(S_IXUSR | f->f_mode)) {
        goto FAILED;
    }

    elf = kcalloc(align_up(f->vnode->size, PAGE_SIZE));
    memcpy(elf, f->vnode->internal, f->vnode->size);
    vfs_close(f);

    if (!check_elf(elf)) {
        goto FAILED;
    }

    size_t entry = elf_entry(elf);
    if (!entry) {
        goto FAILED;
    }

    pd_t *tbl = kcalloc(PAGE_SIZE);

    struct vm_area *vmlist = load_elf_segment(tbl, elf);
    if (!vmlist) {
        goto FAILED;
    }

    kfree(elf);

    /* eret to new user context */
    replace_user_context(tbl, vmlist, entry, argv);
    return 0;

FAILED:
    kfree(elf);

    return -1;
}

SYSCALL_DEFINE2(exec, const char *, path, const char **, argv) {
    return do_exec(path, argv);
}

/* only used for kthread to initialize a userland process */
static void exec_user_context(pd_t *tbl, struct vm_area *vmlist, size_t entry, void *ustack, const char *argv[]) {
    disable_interrupt();

    struct task_struct *ts = current;
    ts->stack = ustack;
    ts->vm_list = vmlist;

    /* set page table */
    write_sysreg(ttbr0_el1, tbl);
    ts->ttbr0 = tbl;

    char *stack_top = init_stack_args(ts->stack + USTACK_SIZE, argv);
    size_t sp_off = stack_top - ts->stack;

    struct pt_regs *trapframe = (struct pt_regs *)((char *)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs));
    memset(trapframe, 0, sizeof(struct pt_regs));

    trapframe->pc = entry;
    trapframe->sp = USTACK_VA + sp_off;
    trapframe->pstate = 0;

    asm(
        "mov sp, %0\n\t"
        "b restore_regs_eret"
    ::"r"(trapframe));
}

int kernel_exec_file(const char *path, const char *argv[]) {
    struct file *f = NULL;
    void *elf = NULL;

    f = vfs_open(path, 0);
    if (!f) {
        return -1;
    }

    if (!(S_IXUSR | f->f_mode)) {
        vfs_close(f);
        return -1;
    }

    elf = kcalloc(align_up(f->vnode->size, PAGE_SIZE));
    memcpy(elf, f->vnode->internal, f->vnode->size);
    vfs_close(f);

    if (!check_elf(elf)) {
        goto FAILED;
    }

    size_t entry = elf_entry(elf);
    if (!entry) {
        goto FAILED;
    }

    /* load elf segments to vm */
    pd_t *tbl = kcalloc(PAGE_SIZE);
    struct vm_area *vmlist = load_elf_segment(tbl, elf);
    void *ustack = alloc_user_stack(tbl, USTACK_SIZE);

    kfree(elf);

    /* instantly eret to userland */
    exec_user_context(tbl, vmlist, entry, ustack, argv);

FAILED:
    kfree(elf);

    return -1;
}