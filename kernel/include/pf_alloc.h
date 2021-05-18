#ifndef _PF_ALLOC_H
#define _PF_ALLOC_H

#define PHY_MEM_ALLOCABLE_START (0x10000000) // before this address are reserved for booting
#define PHY_MEM_ALLOCABLE_END (0x30000000) 
#define PHY_PF_SIZE (0x1000) // page frame size, 4096bytes
#define PF_ENTRY_LENGTH ( \
    (PHY_MEM_ALLOCABLE_END - PHY_MEM_ALLOCABLE_START) / PHY_PF_SIZE \
) // has to be the nth power of 2. 131,072 = 2 ^ 17 in this case

#define ALLOCATED_STATE (100)
#define RESERVED_STATE (200)

struct frame {
    int idx;
    short exp;
    struct frame *next;
};

void init_page_frame();
void *alloc_page(void **addr, short exp);

void mark_as_allocated(struct frame *f);
void mem_stat();
void free_page(void *start, short exp);
void try_merge(short exp);
void append_to_list(struct frame *f, short exp);

#endif