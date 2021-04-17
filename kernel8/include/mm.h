#ifndef	_MM_H
#define	_MM_H



#include "list.h"
#include "utils.h"

#define BUDDY_MAX 				16    	// 4kB ~ 128MB
#define STARTUP_MAX 			16  	// max reserve slot
#define PAGE_SIZE 				4096	//4KB
#define PAGE_SIZE_CTZ 			12
#define INIT_PAGE 				32

extern unsigned long mem_size;


#define pad(x, y) ((((x) + (y)-1) / (y)) * (y))
#define get_order(ptr) (__builtin_ctzl((unsigned long)ptr) - PAGE_SIZE_CTZ)
#define set_buddy_ord(bd, ord) (bd = ord | (bd & 0xe0))
#define set_buddy_flag(bd, flag) (bd = (flag << 5) | (bd & 0x1f))
#define get_buddy_ord(bd) (bd & 0x1f)
#define get_buddy_flag(bd) ((bd & 0xe0) >> 5)
#define ptr_to_pagenum(ptr) (((unsigned long)(ptr)) >> PAGE_SIZE_CTZ)
#define pagenum_to_ptr(pn) ((void *)(((unsigned long)pn) << PAGE_SIZE_CTZ))
#define buddy_pagenum(pg, ord) ((pg) ^ (1 << ord))


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
