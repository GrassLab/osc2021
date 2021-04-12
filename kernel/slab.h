#ifndef _LCD_SLAB_H_
#define _LCD_SLAB_H_

#include "buddy.h"

struct cache_t {
	int size;
	uint64_t *free_head;
} ;

void slab_init();
uint64_t *kmalloc(unsigned int size);
int kfree(uint64_t address);

#endif