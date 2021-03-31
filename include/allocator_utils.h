#ifndef _ALLOCATOR_UTILS_H
#define _ALLOCATOR_UTILS_H

#include "stdint.h"

#define MAX_ELEMENT_NUM 10000
#define MAX_CHUNK_NUM 1000

// Basic Utils
int32_t _cal_bit_len(uint64_t);
uint32_t _get_index_from_mem(uint64_t, uint64_t );

// FreeList Functions
int32_t _find_capable_slot_size(struct _RawFrameArray*, int32_t);
struct FrameListNum* _last_list_element(struct FrameListNum*);
struct FrameListNum* _new_list_element(uint32_t, struct FrameListNum*, struct FrameListNum*);
void _new_frameList_element(struct _RawFrameArray*, int32_t, uint32_t);
void _rm_frameList_element(struct _RawFrameArray*, struct FrameListNum*, int);
void _merge_frameList_element(struct _RawFrameArray*);

// Chunk Functions
struct FrameChunk* _new_chunk_from_idx(uint32_t, struct FrameChunk*, struct FrameChunk*);
struct FrameChunk* _new_chunk_from_zero(struct _RawFrameArray*, struct FrameChunk*);
void _rm_chunk(struct FrameChunk*);

#endif