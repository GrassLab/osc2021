#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H

#include "stdint.h"
#define FREE_SLOT -1
#define UNFREE_SLOT -2
#define MAX_ELEMENT_NUM 10000

struct FrameListNum {
    uint32_t index;
    struct FrameListNum *next;
};

typedef struct _RawFrameArray{
    uint64_t base_addr, end_addr;
    int16_t val[0x10000 + 100];
    struct FrameListNum *freeList[20];

    uint32_t (*new_memory)(struct _RawFrameArray*, uint64_t);
} FrameArray;

FrameArray* NewFrameArray();
uint32_t new_memory(struct _RawFrameArray*, uint64_t);

#endif