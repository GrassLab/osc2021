#ifndef MM_H
#define MM_H
#include "data_type.h"
void startup_allocator_init ();
void *startup_malloc (u64 size);
void startup_aligned (u64 size);
void startup_lock_memory (u64 start, u64 end);


/* TODO: temporary functions */
void show_list();
void startup_used_list_reorder ();
#endif
