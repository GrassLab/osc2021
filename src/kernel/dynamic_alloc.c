#include "dynamic_alloc.h"
#include "def.h"
#include "pf_alloc.h"
#include "io.h"

struct pool mem_pool;

void init_dym_alloc()
{
    mem_pool.chunk_256 = NULL;
    mem_pool.chunk_128 = NULL;
    mem_pool.chunk_64  = NULL;
    mem_pool.chunk_32  = NULL;
    mem_pool.chunk_16  = NULL;
}

void *malloc(int size)
{
    int nearest_size;
    // check there is enough space in pool
    nearest_size = size <= 16 ? 16 : size <= 32 ? 32 : size <= 64 ? 64 : size <= 128 ? 128 : size <= 256 ? 256 : 0;
    if (nearest_size == 0) {
      
        printf("cant alloc this size\r\n");
        return NULL;
    }

    return get_chunk(nearest_size);
}

void *get_chunk(int chunk_size)
{
    switch (chunk_size)
    {
    case 256:
        if (mem_pool.chunk_256) {
            struct chunk_256 *rtn = mem_pool.chunk_256;
            mem_pool.chunk_256 = mem_pool.chunk_256->next;
            return (void *)rtn;
        }
        break;
    case 128:
        if (mem_pool.chunk_128) {
            struct chunk_128 *rtn = mem_pool.chunk_128;
            mem_pool.chunk_128 = mem_pool.chunk_128->next;
            return (void *)rtn;
        }
        break;
    case 64:
        if (mem_pool.chunk_64) {
            struct chunk_64 *rtn = mem_pool.chunk_64;
            mem_pool.chunk_64 = mem_pool.chunk_64->next;
            return (void *)rtn;
        }
        break;
    case 32:
        if (mem_pool.chunk_32) {
            struct chunk_32 *rtn = mem_pool.chunk_32;
            mem_pool.chunk_32 = mem_pool.chunk_32->next;
            return (void *)rtn;
        }
        break;
    case 16:
        if (mem_pool.chunk_16) {
            struct chunk_16 *rtn = mem_pool.chunk_16;
            mem_pool.chunk_16 = mem_pool.chunk_16->next;
            return (void *)rtn;
        }
        break;
    
    default:
        return NULL;
        break;
    }

    //if reach here, means no fit chunk in pool
    augment_pool();
    return get_chunk(chunk_size);
}


void augment_pool()
{
    printf("augmenting...\r\n");
    struct dym_frame *new_frame = NULL;
    alloc_page((void *)&new_frame, 1);
    
    /** chunk 256 **/
    // parse
    for (int i = 0; i < 3; i++)
    {
        new_frame->chunk_256[i].next = &new_frame->chunk_256[i+1];
    }
    new_frame->chunk_256[2].next = NULL;
    
    //augment
    append_to_pool(256, new_frame->chunk_256);

    /** chunk 128 **/
    // parse
    for (int i = 0; i < 6; i++)
    {
        new_frame->chunk_128[i].next = &new_frame->chunk_128[i+1];
    }
    new_frame->chunk_128[5].next = NULL;
    
    //augment
    append_to_pool(128, new_frame->chunk_128);

    /** chunk 64 **/
    // parse
    for (int i = 0; i < 12; i++)
    {
        new_frame->chunk_64[i].next = &new_frame->chunk_64[i+1];
    }
    new_frame->chunk_64[11].next = NULL;
    
    //augment
    append_to_pool(64, new_frame->chunk_64);

    /** chunk 32 **/
    // parse
    for (int i = 0; i < 24; i++)
    {
        new_frame->chunk_32[i].next = &new_frame->chunk_32[i+1];
    }
    new_frame->chunk_32[23].next = NULL;
    
    //augment
    append_to_pool(32, new_frame->chunk_32);

    /** chunk 16 **/
    // parse
    for (int i = 0; i < 64; i++)
    {
        new_frame->chunk_16[i].next = &new_frame->chunk_16[i+1];
    }
    new_frame->chunk_16[63].next = NULL;
    
    //augment
    append_to_pool(16, new_frame->chunk_16);

    printf("augmented!\r\n");
}

void pool_stat()
{
    int count = 0;
    struct chunk_256 *tmp_256 = mem_pool.chunk_256;
    while (tmp_256) {
        count++;
        tmp_256 = tmp_256->next;
    }
    printf("chunk 256: %d\r\n", count);

    count = 0;
    struct chunk_128 *tmp_128 = mem_pool.chunk_128;
    while (tmp_128) {
        count++;
        tmp_128 = tmp_128->next;
    }
    printf("chunk 128: %d\r\n", count);

    count = 0;
    struct chunk_64 *tmp_64 = mem_pool.chunk_64;
    while (tmp_64) {
        count++;
        tmp_64 = tmp_64->next;
    }
    printf("chunk 64: %d\r\n", count);

    count = 0;
    struct chunk_32 *tmp_32 = mem_pool.chunk_32;
    while (tmp_32) {
        count++;
        tmp_32 = tmp_32->next;
    }
    printf("chunk 32: %d\r\n", count);
    
    count = 0;
    struct chunk_16 *tmp_16 = mem_pool.chunk_16;
    while (tmp_16) {
        count++;
        tmp_16 = tmp_16->next;
    }
    printf("chunk 16: %d\r\n", count);
}

void free(void *addr)
{
    // mod 4kb and determin what size it is by offset
    int offset = (int)(long)addr % PHY_PF_SIZE;
    if (offset < 768) {
        // 256
        struct chunk_256 *chunk = (struct chunk_256 *)addr;
        chunk->next = NULL;
        append_to_pool(256, chunk);
        // printf("it belongs to %d\r\n", 256);
    } else if (offset < 1536) {
        // 128
        struct chunk_128 *chunk = (struct chunk_128 *)addr;
        chunk->next = NULL;
        append_to_pool(128, chunk);
        // printf("it belongs to %d\r\n", 128);
    } else if (offset < 2304) {
        // 64
        struct chunk_64 *chunk = (struct chunk_64 *)addr;
        chunk->next = NULL;
        append_to_pool(64, chunk);
        // printf("it belongs to %d\r\n", 64);
    } else  if (offset < 3072) {
        // 32
        struct chunk_32 *chunk = (struct chunk_32 *)addr;
        chunk->next = NULL;
        append_to_pool(32, chunk);
        // printf("it belongs to %d\r\n", 32);
    } else {
        // 16
        struct chunk_16 *chunk = (struct chunk_16 *)addr;
        chunk->next = NULL;
        append_to_pool(16, chunk);
        // printf("it belongs to %d\r\n", 16);
    }

}

void append_to_pool(int size, void* record)
{
    if (size == 256) {
        if (mem_pool.chunk_256) {
            struct chunk_256 *tmp_256 = mem_pool.chunk_256;
            while (tmp_256->next) {
                tmp_256 = tmp_256->next;
            }
            tmp_256->next = record;
        } else {
            mem_pool.chunk_256 = record;
        }
    } else if (size == 128) {
        if (mem_pool.chunk_128) {
            struct chunk_128 *tmp_128 = mem_pool.chunk_128;
            while (tmp_128->next) {
                tmp_128 = tmp_128->next;
            }
            tmp_128->next = record;
        } else {
            mem_pool.chunk_128 = record;
        }
    } else if (size == 64) {
        if (mem_pool.chunk_64) {
            struct chunk_64 *tmp_64 = mem_pool.chunk_64;
            while (tmp_64->next) {
                tmp_64 = tmp_64->next;
            }
            tmp_64->next = record;
        } else {
            mem_pool.chunk_64 = record;
        }
    } else if (size == 32) {
        if (mem_pool.chunk_32) {
            struct chunk_32 *tmp_32 = mem_pool.chunk_32;
            while (tmp_32->next) {
                tmp_32 = tmp_32->next;
            }
            tmp_32->next = record;
        } else {
            mem_pool.chunk_32 = record;
        }
    } else if (size == 16) {
        if (mem_pool.chunk_16) {
            struct chunk_16 *tmp_16 = mem_pool.chunk_16;
            while (tmp_16->next) {
                tmp_16 = tmp_16->next;
            }
            tmp_16->next = record;
        } else {
            mem_pool.chunk_16 = record;
        }
    }
}