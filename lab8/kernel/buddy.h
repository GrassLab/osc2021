#ifndef BUDDY_H
#define BUDDY_H

#include "mmu.h"

typedef int bool;
#define true 1
#define false 0

#define MEMORY_START			(0x10000000 + KVA)	
#define MEMORY_SIZE				0x10000000	// from TA, 0x10000000-0x20000000 
#define PAGE_SIZE				0x1000		// 4096
#define PAGE_NUM				(MEMORY_SIZE / PAGE_SIZE) // all page frame = 65536
#define BUDDY_MAX_ORDER 		16			// 2^0 ~ 2^16
#define BUDDY_ALLOCATED_NUM 	65536
#define BUDDY_NODE_LIST_NUM 	16

enum Status
{
	FREE,
	USED
};

struct buddy_node
{
	int start;
	int end;
	enum Status status;
};

struct buddy_head
{
	int count;
	struct buddy_node node_list[BUDDY_NODE_LIST_NUM];
};

struct buddy_allocated
{
	int count;
	struct buddy_node node_list[BUDDY_ALLOCATED_NUM];
};


void buddy_init();
int buddy_alloc(int size);
void buddy_free(int node_start);
void buddy_test();

#endif