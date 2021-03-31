#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H

#define PRINT_ALLOCATE_LOG 1

#include "stdint.h"
#define FREE_SLOT -1
#define UNFREE_SLOT -2
#define ALLOCATED_SLOT_SHIFT 100
#define CHUNK_SLOT -3

struct FrameListNum {
    uint32_t index;
    struct FrameListNum *next;
    struct FrameListNum *prev;
};

struct FrameChunk {
    uint32_t index;
    uint8_t chunk16[32];    // base=0
    uint8_t chunk32[32];    // base=512
    uint8_t chunk64[16];    // base=1536
    uint8_t chunk128[6];    // base=2560
    uint8_t chunk256[3];    // base=3328
    uint8_t free_chunk_num;
    struct FrameChunk *next;
    struct FrameChunk *prev;
};

typedef struct _RawFrameArray{
    uint64_t base_addr, end_addr;
    int32_t val[0x10000 + 100];
    struct FrameListNum *freeList[20];
} FrameArray;

FrameArray* NewFrameArray();
uint64_t kmalloc(uint64_t);
void free(uint64_t);
uint64_t _new_frame(uint64_t, int32_t, int32_t, uint8_t);

#endif