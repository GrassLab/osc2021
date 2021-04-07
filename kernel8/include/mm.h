#ifndef	_MM_H
#define	_MM_H
#include "list.h"
#include "utils.h"

extern unsigned long mem_size;

#define BUDDY_MAX 				16    	// 4kB ~ 128MB
#define STARTUP_MAX 			16  	// max reserve slot
#define PAGE_SIZE 				4096	//4KB
#define PAGE_SIZE_CTZ 			12
#define INIT_PAGE 				32

#define pad(x, y) ((((x) + (y)-1) / (y)) * (y))
#define get_order(ptr) (__builtin_ctzl((unsigned long)ptr) - PAGE_SIZE_CTZ)
#define set_buddy_ord(bd, ord) (bd = ord | (bd & 0xe0))
#define set_buddy_flag(bd, flag) (bd = (flag << 5) | (bd & 0x1f))
#define get_buddy_ord(bd) (bd & 0x1f)
#define get_buddy_flag(bd) ((bd & 0xe0) >> 5)
#define ptr_to_pagenum(ptr) (((unsigned long)(ptr)) >> PAGE_SIZE_CTZ)
#define pagenum_to_ptr(pn) ((void *)(((unsigned long)pn) << PAGE_SIZE_CTZ))
#define buddy_pagenum(pg, ord) ((pg) ^ (1 << ord))

enum{
	BUDDY_FREE,  			//0
	BUDDY_USE,				//1
	SLAB_USE,				//2
	RESRVE_USE				//3
};


typedef struct buddy_system {
  list_head free_list[BUDDY_MAX];
} buddy_system;

typedef struct startup_allocator {
  unsigned long addr[STARTUP_MAX];
  unsigned long size[STARTUP_MAX];
} startup_allocator;

typedef struct cache_list {
  struct cache_list *next;
} cache_list;

typedef struct page_descriptor {
  void *page;
  struct page_descriptor *next_pd;
  cache_list *free_list;
  unsigned int free_count;
} page_descriptor;

typedef struct slab_cache {
  struct slab_cache *next_slab;
  page_descriptor *head_pd;
  page_descriptor *cache_pd;
  void *page_slice_pos;
  unsigned int size;
  unsigned int free_count;
  unsigned int page_remain;
} slab_cache;

buddy_system 		buddy;
startup_allocator 	startup;
char *				buddy_stat;
slab_cache *		slab_st;
slab_cache *		sc_slab_tok;
slab_cache *		pd_slab_tok;

void *kmalloc(unsigned long size);
void *alloc_page(unsigned int size);
void *alloc_slab(void *slab_tok);

void kfree(void *ptr);
void free_page(void *ptr);
void free_reserve(void *ptr);
void free_unknow_slab(void *ptr);

void init_memory_system();

void print_buddy_info();
void print_buddy_stat();
void print_slab();

void test_buddy();
void test_slab();


#endif  /*_BUDDY_H */
