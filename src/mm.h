#ifndef MM_H
#define MM_H
#include "data_type.h"

extern void * (*m_malloc) (u64 size);
extern void (*m_free) (void *addr);

/* startup allocator */
void startup_allocator_init ();
void *startup_malloc (u64 size);
void startup_free (void *addr);
void startup_aligned (u64 size);
void startup_lock_memory (u64 start, u64 end);

/* buddy system */
void buddy_system_init ();
void buddy_system_show_buckets ();
void *bs_malloc (u64 size);
void bs_free (void *addr);

/* dynamic allocator */
void show_malloc_bins ();
void dynamic_allocator_init ();

/* TODO: temporary functions */
void show_list();
void startup_used_list_reorder ();
void buddy_system_show_entry_table ();

void bs_free_interface (char *buffer);
void bs_malloc_interface (char *buffer);

void m_free_interface (char *buffer);
void m_malloc_interface (char *buffer);

char *strdup(char *str);
#endif
