#ifndef _SLAB_H
#define _SLAB_H
#define MAX_KEM_CACHE 10
struct slab {
    struct slab *next;
    struct block *use_block; 
    int inuse;
    int free;
    int obj_size;
    unsigned long s_mem;
};

struct kmem_cache {
    struct kmem_cache *next;
    struct slab *slab_head;
    struct slab *slab_tail;
    int num_slab;
    int obj_size;
};

struct kmem_cache_list {
    struct kmem_cache *head;
    struct kmem_cache *tail;
};


unsigned long obj_allocate(int size);
unsigned long kmem_cache_alloc(struct kmem_cache *kmemche);
struct kmem_cache *kmem_cache_create(int size);

#endif