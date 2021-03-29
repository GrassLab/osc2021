#ifndef MM_H
#define MM_H
#include "data_type.h"
/* startup allocator */
void startup_allocator_init ();
void *startup_malloc (u64 size);
void startup_aligned (u64 size);
void startup_lock_memory (u64 start, u64 end);

/* buddy system */
void buddy_system_init ();
void buddy_system_show_buckets ();
void *bs_malloc (u64 size);


/* TODO: temporary functions */
void show_list();
void startup_used_list_reorder ();
void buddy_system_show_entry_table ();
#endif
