#include "stdint.h"
#include "mini_uart.h"
#include "allocator.h"
#include "str_tool.h"
#include "allocator_utils.h"

extern FrameArray *frame_array;
struct FrameListNum freeListElement[MAX_ELEMENT_NUM];
struct FrameChunk freeChunkElement[MAX_CHUNK_NUM];
uint32_t cur_element_idx = 0;
uint32_t cur_chunk_idx = 0;
struct FrameChunk *chunk_list = 0;

// Basic Utils
int32_t _cal_bit_len(uint64_t num){
    int32_t res = 0;
    while(num){
        num >>= 1;
        res += 1;
    }
    return res;
}

uint32_t _get_index_from_mem(uint64_t base_addr, uint64_t addr){
    return (addr - base_addr) >> 12;   // divide by 4096
}

// FreeList Functions
int32_t _find_capable_slot_size(int32_t need_size_power){
    int32_t find_size_power = need_size_power;
    while(1){
        if(find_size_power > 19){
            uart_puts("ERROR: new_memory cannot find suitable free memory!\r\n");
            return -1;
        }
        if(frame_array->freeList[find_size_power])
            break;
        find_size_power += 1;
    }
    return find_size_power;
}

struct FrameListNum* _last_list_element(struct FrameListNum *header){
    while(header->next)
        header = header->next;
    return header;
}

struct FrameListNum* _new_list_element(uint32_t index, struct FrameListNum *next, struct FrameListNum *prev){
    freeListElement[cur_element_idx].index = index;
    freeListElement[cur_element_idx].next = next;
    freeListElement[cur_element_idx].prev = prev;
    cur_element_idx += 1;
    return &freeListElement[cur_element_idx-1];
}

void _new_frameList_element(int32_t power_idx, uint32_t element_idx){
    struct FrameListNum *cursor;
    if(!frame_array->freeList[power_idx]){
        frame_array->freeList[power_idx] = _new_list_element(element_idx, 0, 0);
    }
    else{
        cursor = _last_list_element(frame_array->freeList[power_idx]);
        cursor->next = _new_list_element(element_idx, 0, cursor);
    }
}

void _rm_frameList_element(struct FrameListNum *cursor, int cur_idx){
    if(!cursor->prev){
        frame_array->freeList[cur_idx] = cursor->next;
        cursor->next->prev = 0;
    }
    else{
        cursor->prev->next = cursor->next;
        if(cursor->next)
            cursor->next->prev = cursor->prev;
    }
}

void _merge_frameList_element(){
    uint8_t i;
    uint32_t min_val;
    uint64_t cur_base;
    struct FrameListNum *left, *right;
    for(i=0; i<20; i++){
        cur_base = 1<<(i+1);
        left = frame_array->freeList[i];
        while(left){
            right = left->next;
            while(right){
                if((left->index^right->index) < cur_base){
                    min_val = left->index<right->index?left->index:right->index;
                    _new_frameList_element(i+1, min_val);
                    _rm_frameList_element(left, i);
                    _rm_frameList_element(right, i);
                    if(PRINT_ALLOCATE_LOG){
                        uart_puts("- - - - - - - - - -\r\n");
                        uart_puts("Merge Memory Block: from idx ");
                        uart_puts(itoa(min_val, 10));
                        uart_puts(" to idx ");
                        uart_puts(itoa(min_val+cur_base-1, 10));
                        uart_puts("\r\n");
                    }
                    if(left->next == right)
                        left = left->next;
                    break;
                }
                right = right->next;
            }
            left = left->next;
        }
    }
}


// Chunk Functions
struct FrameChunk* _new_chunk_from_idx(uint32_t index, struct FrameChunk *next, struct FrameChunk *prev){
    freeChunkElement[cur_chunk_idx].index = index;
    freeChunkElement[cur_chunk_idx].next = next;
    freeChunkElement[cur_chunk_idx].prev = prev;
    freeChunkElement[cur_chunk_idx].free_chunk_num = 89;
    uint8_t i;
    for(i=0; i<32; i++){
        freeChunkElement[cur_chunk_idx].chunk16[i] = 0;
        freeChunkElement[cur_chunk_idx].chunk32[i] = 0;
    }
    for(i=0; i<16; i++)
        freeChunkElement[cur_chunk_idx].chunk64[i] = 0;
    for(i=0; i<6; i++)
        freeChunkElement[cur_chunk_idx].chunk128[i] = 0;
    for(i=0; i<3; i++)
        freeChunkElement[cur_chunk_idx].chunk256[i] = 0;
    cur_chunk_idx += 1;
    return &freeChunkElement[cur_chunk_idx-1];
}

struct FrameChunk* _new_chunk_from_zero(struct FrameChunk *prev){
    uint64_t base_addr = _new_frame(0, 0, _find_capable_slot_size(0), 1);
    uint32_t index = _get_index_from_mem(frame_array->base_addr, base_addr);
    return _new_chunk_from_idx(index, 0, prev);
}

void _rm_chunk(struct FrameChunk *cursor){
    if(!cursor->prev){
        chunk_list = cursor->next;
        cursor->next->prev = 0;
    }
    else{
        cursor->prev->next = cursor->next;
        if(cursor->next)
            cursor->next->prev = cursor->prev;
    }
}