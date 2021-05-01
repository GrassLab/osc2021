#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define OBJ_ALLOCATOR_PAGE_SIZE 		16				// pool allow allocate max page count
#define ALLOCATOR_POOL_SIZE				32

struct object_allocator
{
	unsigned int max_pool[OBJ_ALLOCATOR_PAGE_SIZE];		// record max obj slot count (page size / obj size)
	unsigned int cur_pool[OBJ_ALLOCATOR_PAGE_SIZE];		// record current obj use slot count
	unsigned int page[OBJ_ALLOCATOR_PAGE_SIZE];			// record page index
	unsigned int page_count;							// record page count
	unsigned int obj_size;								// obj size (ex. 64,512)
	unsigned int max_pool_init;							// initial max_pool
};

// has 4 different obj size
struct dynamic_allocator
{
	struct object_allocator *obj_allocator_64;
	struct object_allocator *obj_allocator_512;
	struct object_allocator *obj_allocator_2048;
	struct object_allocator *obj_allocator_4096;
};

struct object_allocator* object_allocator_init(int size);
unsigned long long obj_alloc(struct object_allocator *self);
int obj_allocator_free(struct object_allocator *self, unsigned long long addr);

struct dynamic_allocator* dynamic_allocator_init();
unsigned long long dynamic_alloc(struct dynamic_allocator *self, int req_size);
void dynamic_free(struct dynamic_allocator *self, unsigned long long addr);
void dynamic_test();

void memory_init();
#endif