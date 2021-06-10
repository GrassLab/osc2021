#ifndef __MMU_H_
#define __MMU_H_
#include <stddef.h>

#define PHYS_OFFSET 0xffff000000000000
#define PAGE_SIZE 0x1000
#define USTACK_VA 0x7fffff000000
#define PAR_PA_MASK 0xffffffff000L
#define ENTRY_PA_MASK 0xfffffffff000L

static inline unsigned long virt_to_phys(void *p) {
    return (unsigned long)p - PHYS_OFFSET;
}

static inline void *phys_to_virt(unsigned long p) {
    return (void *)(p + PHYS_OFFSET);
}

typedef unsigned long pd_t;

struct vm_area {
    struct vm_area *next;
    size_t va;
    size_t size;
    size_t flags;
};

void setup_kernel_space_mapping();
void setup_identity_mapping();
void map_user_page(pd_t *tbl, unsigned long va, unsigned long pa, int pflags);
void *alloc_user_stack(pd_t *tbl, unsigned long size);
void switch_mm();
size_t get_PTE(size_t va);
void free_user_vm(pd_t *tbl);
void free_vm_area(struct vm_area *vm);

#endif