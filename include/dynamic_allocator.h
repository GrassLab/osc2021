#ifndef __DYNAMIC_ALLOCATOR_H__
#define __DYNAMIC_ALLOCATOR_H__
#include "buddy.h"
#include "system.h"

/*
    chunk header: size, is_used
*/
#define CHUNK_HEADER_SIZE 8
#define USE_FLAG 0x01
#define UNUSE_FLAG 0x00
typedef struct DMA_header
{
    char *prev;
    char *next;
    uint32_t size;
    uint32_t info; // [mem_entry, block_exp, used]
    uint32_t foo;

} DMA_header;
void dynamic_allocator_init();
void *dynamic_alloc(uint32_t);
uint32_t __encode_DMA_info(uint8_t, uint8_t, uint8_t);
uint32_t __decode_DMA_block(uint32_t);
uint8_t __address_to_entry(uint64_t);
uint8_t __memory_unused(uint32_t);
void __mark_memory_unused(DMA_header *);
void __split_chunk(DMA_header *, uint32_t);
uint8_t __dma_merge(DMA_header *, DMA_header *);
uint8_t __check_memory_releasable(DMA_header *);
uint32_t __decode_block_size(uint32_t);
uint8_t __same_block(uint32_t, uint32_t);
void __log_chunk_merge(uint64_t, uint64_t);
void __log_block_release(DMA_header *);
void dynamic_free(void *);
char *find_proper_page(int);
uint32_t __align_8byte(uint32_t);
void show_memory_pool();

void DMA_test();
void DMA_test2();
#endif
