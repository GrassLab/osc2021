#include "slab.h"
#include "mm.h"
#include "printf.h"
#define pfn_to_phy(pfn) LOW_MEMORY + (pfn)*PAGE_SIZE
#define pfn_to_vir(pfn) LOW_MEMORY + (pfn)*PAGE_SIZE + VA_START

static struct kmem_cache_list kmemche_l;
static int kmem_cache_record[MAX_KEM_CACHE]; // record the used of block
static struct kmem_cache kmemches[MAX_KEM_CACHE];

// return the index of free block index
int find_kmem_cache()
{
   for (int i = 0 ; i < MAX_KEM_CACHE ; i++) {
       if (kmem_cache_record[i] == 0) {
           kmem_cache_record[i] = 1;
           return i;
       }
   } 
   return -1;
}

// init new kmemches
void init_kmem_cache(struct kmem_cache *kmemche, int size)
{
    kmemche->slab_head = 0;
    kmemche->obj_size = size;
    kmemche->num_slab = 0;
    return;
}

// insert kmemche_l
void insert_kmemche_l(struct kmem_cache *kmemche) 
{
    if(kmemche_l.head == 0) {
        kmemche_l.head = kmemche;
        kmemche_l.tail = kmemche;
        kmemche_l.head->next = 0;
    }
    else {
        kmemche_l.tail->next = kmemche;
        kmemche_l.tail = kmemche;
    }
    return;
}

struct slab *init_slab(struct block *alloc_block, int obj_size) 
{
    struct slab *new_slab = pfn_to_vir(alloc_block->pfn);
    int object_num = (PAGE_SIZE - sizeof(struct slab)) / (sizeof(int) + obj_size);
    new_slab->use_block = alloc_block;
    new_slab->inuse = object_num;
    int *kmem_bufctl = (int *)(new_slab + 1);
    for(int i = 0 ; i < object_num; i++) {
        *(kmem_bufctl + i) = i + 1;
    }
    new_slab->free = 0;
    new_slab->next = 0;
    new_slab->s_mem = (unsigned long)(kmem_bufctl + object_num);
    new_slab->obj_size = obj_size;
    return new_slab;
}

struct kmem_cache *kmem_cache_create(int size) 
{
    // find free kmemches
    int free_index = find_kmem_cache();
    // init kmemches
    init_kmem_cache(&kmemches[free_index], size);
    // insert into kmemche_l
    insert_kmemche_l(&kmemches[free_index]);
    return &kmemches[free_index];
}


void insert_slab(struct kmem_cache *kmemche, struct slab *alloc_slab)
{
    if(kmemche->slab_head == 0) {
        kmemche->slab_head = alloc_slab;
        kmemche->slab_tail = alloc_slab;        
        kmemche->next = 0;
    }
    else {
        kmemche->slab_tail->next = alloc_slab;
        kmemche->slab_tail = alloc_slab;
    }
    kmemche->num_slab += 1;
    return;
}

struct slab *slab_create(struct kmem_cache *kmemche)
{
    // find a block
    struct block *alloc_block = get_free_pages(1); //warning page num update
    // init slab
    int obj_size = kmemche->obj_size;
    struct slab *alloc_slab = init_slab(alloc_block, obj_size);
    // insert to kmem_cache
    insert_slab(kmemche, alloc_slab);
    return alloc_slab;
}

unsigned long slab_get_obj(struct slab *alloc_slab, int obj_size)
{
    unsigned long objp = alloc_slab->s_mem + (alloc_slab->free * obj_size);
    int *kmem_bufctl = (int *)(alloc_slab + 1);
    int next = kmem_bufctl[alloc_slab->free];
    alloc_slab->inuse -= 1;
    if(alloc_slab->inuse == 0) {
        alloc_slab->free = -1;
    }
    else {
        alloc_slab->free = next;
    }
    return objp;
}

void slab_put_obj(unsigned long obj_addr)
{
    // find slab
    struct slab *slab_ptr = obj_addr & PAGE_MASK;
    // find obj number
    int objnr = (obj_addr - slab_ptr->s_mem) / slab_ptr->obj_size;
    int *kmem_bufctl = (int *)(slab_ptr + 1);
    kmem_bufctl[objnr] = slab_ptr->free;
    slab_ptr->free = objnr;
    slab_ptr->inuse += 1;
}

unsigned long kmem_cache_alloc(struct kmem_cache *kmemche)
{
    struct slab *slab_ptr = kmemche->slab_head;
    while((slab_ptr != 0) && (slab_ptr->inuse == 0)) {
        slab_ptr = slab_ptr->next;
    }
    if(slab_ptr == 0) {
        slab_ptr = slab_create(kmemche);
    }
    return slab_get_obj(slab_ptr, kmemche->obj_size);
}


unsigned long obj_allocate(int size) 
{
    if(size >= 2049) {
        unsigned long new_block = get_free_pages(1 + (size/PAGE_SIZE));
        return new_block;
    }
    struct kmem_cache *kmemche_ptr = kmemche_l.head;
    while((kmemche_ptr != 0) && (size != kmemche_ptr->obj_size)) {
        kmemche_ptr = kmemche_ptr->next;
    }
    if(kmemche_ptr == 0) {
        // kmem_cache not found
        // allocate kmem_cache
        kmemche_ptr = kmem_cache_create(size);
    }
    // find slab
    struct slab *slab_ptr = kmemche_ptr->slab_head;
    while((slab_ptr != 0) && (slab_ptr->inuse == 0)) {
        slab_ptr = slab_ptr->next;
    }
    if(slab_ptr == 0) {
        slab_ptr = slab_create(kmemche_ptr);
    }
    return slab_get_obj(slab_ptr, size);
}

void init_obj_allocator() 
{
    memzero((unsigned long)kmem_cache_record, MAX_KEM_CACHE * sizeof(int));
    memzero((unsigned long)kmemches, MAX_KEM_CACHE * sizeof(struct kmem_cache));
    kmemche_l.head = 0;
    kmemche_l.tail = 0;
}