#include "stdint.h"
#include "mini_uart.h"
#include "allocator.h"
#include "str_tool.h"

struct FrameListNum freeListElement[MAX_ELEMENT_NUM];
struct FrameChunk freeChunkElement[MAX_CHUNK_NUM];
uint32_t cur_element_idx = 0;
uint32_t cur_chunk_idx = 0;
struct FrameChunk *chunk_list = 0;

int32_t _cal_log_2(uint64_t num){
    int32_t res = 0;
    while(num){
        num >>= 1;
        res += 1;
    }
    return res;
}

struct FrameListNum* _get_last_list_element(struct FrameListNum *header){
    while(header->next)
        header = header->next;
    return header;
}

struct FrameChunk* _get_new_chunk_element(uint32_t index, struct FrameChunk *next, struct FrameChunk *prev){
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

struct FrameListNum* _get_new_list_element(uint32_t index, struct FrameListNum *next, struct FrameListNum *prev){
    freeListElement[cur_element_idx].index = index;
    freeListElement[cur_element_idx].next = next;
    freeListElement[cur_element_idx].prev = prev;
    cur_element_idx += 1;
    return &freeListElement[cur_element_idx-1];
}

void _insert_to_frameList(struct _RawFrameArray *frame_array, int32_t power_idx, uint32_t element_idx){
    struct FrameListNum *cursor;
    if(!frame_array->freeList[power_idx]){
        frame_array->freeList[power_idx] = _get_new_list_element(element_idx, 0, 0);
    }
    else{
        cursor = _get_last_list_element(frame_array->freeList[power_idx]);
        cursor->next = _get_new_list_element(element_idx, 0, cursor);
    }
}

uint64_t _allocate_slot(struct _RawFrameArray *frame_array, uint64_t need_size, int32_t need_size_power, int32_t find_size_power, uint8_t is_chunk){
    uint32_t index = frame_array->freeList[find_size_power]->index;
    uint64_t return_addr = frame_array->base_addr + (4096 * (uint64_t)index);
    frame_array->freeList[find_size_power] = frame_array->freeList[find_size_power]->next;
    uint32_t cur_end_index;
    int32_t cur_size_power;

    if(find_size_power != need_size_power){
        // Process freeList
        cur_end_index = index + (1<<find_size_power);
        cur_size_power = find_size_power - 1;

        while(cur_size_power >= need_size_power){
            cur_end_index = (cur_end_index + index)/2;
            _insert_to_frameList(frame_array, cur_size_power, cur_end_index);

            // Process val array
            frame_array->val[cur_end_index] = cur_size_power;
            cur_size_power -= 1;
        }
    }
    
    uint32_t cur_index=index, assigned_block_num=1<<need_size_power;
    cur_end_index = index + assigned_block_num;
    
    if(is_chunk)
        frame_array->val[cur_index] = CHUNK_SLOT;
    else{
        frame_array->val[cur_index] = ALLOCATED_SLOT_SHIFT + need_size;
        cur_index += 1;
        for(;cur_index<cur_end_index; cur_index++)
            frame_array->val[cur_index] = UNFREE_SLOT;
    }

    if(PRINT_ALLOCATE_LOG){
        // Print Log
        uart_puts("- - - - - - - - - -\r\n");
        uart_puts("New Memory: from idx ");
        uart_puts(itoa(index, 10));
        uart_puts(" to idx ");
        uart_puts(itoa(cur_end_index-1, 10));
        uart_puts("\r\n");
    }

    uint32_t redundant_block_num=assigned_block_num-need_size-1, base=assigned_block_num>>1, idx=need_size_power-1;
    while(base){
        if(redundant_block_num&base){
            cur_end_index -= base;
            // Process freeList
            _insert_to_frameList(frame_array, idx, cur_end_index);

            // Process val array
            frame_array->val[cur_end_index] = idx;
            for(cur_index=cur_end_index+1; cur_index<cur_end_index+base; cur_index++)
                frame_array->val[cur_index] = FREE_SLOT;

            if(PRINT_ALLOCATE_LOG){
                // Print Log
                uart_puts("- - - - - - - - - -\r\n");
                uart_puts("Free Redundant Block: from idx ");
                uart_puts(itoa(cur_end_index, 10));
                uart_puts(" to idx ");
                uart_puts(itoa(cur_end_index+base-1, 10));
                uart_puts("\r\n");
            }
            redundant_block_num ^= base;
        }
        base >>= 1;
        idx -= 1;
    }
    if(PRINT_ALLOCATE_LOG)
        uart_puts("- - - - - - - - - -\r\n");

    return return_addr;
}

void _rm_free_list_element(struct _RawFrameArray *self, struct FrameListNum *cursor, int cur_idx){
    if(!cursor->prev){
        self->freeList[cur_idx] = cursor->next;
        cursor->next->prev = 0;
    }
    else{
        cursor->prev->next = cursor->next;
        if(cursor->next)
            cursor->next->prev = cursor->prev;
    }
}

void _merge_free_list(struct _RawFrameArray *self){
    uint8_t i;
    uint32_t min_val;
    uint64_t cur_base;
    struct FrameListNum *left, *right;
    for(i=0; i<20; i++){
        cur_base = 1<<(i+1);
        left = self->freeList[i];
        while(left){
            right = left->next;
            while(right){
                if((left->index^right->index) < cur_base){
                    min_val = left->index<right->index?left->index:right->index;
                    _insert_to_frameList(self, i+1, min_val);
                    _rm_free_list_element(self, left, i);
                    _rm_free_list_element(self, right, i);
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

uint32_t _get_index_from_mem(uint64_t base_addr, uint64_t addr){
    return (addr - base_addr) >> 12;   // divide by 4096
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

void _free_chunk(struct _RawFrameArray *self, uint64_t free_addr, uint32_t index){
    struct FrameChunk *cursor = chunk_list;
    while(cursor->index != index)
        cursor = cursor->next;
    uint64_t shift_addr = free_addr - self->base_addr - (4096 * (uint64_t)index);
    if(shift_addr<512){
        // chunk16
        if(cursor->chunk16[shift_addr/16] != 1)
            uart_puts("ERROR! Value of Chunk is not 1\r\n");
        else{
            cursor->chunk16[shift_addr/16] = 0;
            uart_puts("Free chunk16\r\n");
        }
        goto CHECK_EMPTY_CHUNK;
    }

    shift_addr -= 512;
    if(shift_addr<1024){
        // chunk32
        if(cursor->chunk32[shift_addr/32] != 1)
            uart_puts("ERROR! Value of Chunk is not 1\r\n");
        else{
            cursor->chunk32[shift_addr/32] = 0;
            uart_puts("Free chunk32\r\n");
        }
        goto CHECK_EMPTY_CHUNK;
    }

    shift_addr -= 1024;
    if(shift_addr<1024){
        // chunk64
        if(cursor->chunk64[shift_addr/64] != 1)
            uart_puts("ERROR! Value of Chunk is not 1\r\n");
        else{
            cursor->chunk64[shift_addr/64] = 0;
            uart_puts("Free chunk64\r\n");
        }
        goto CHECK_EMPTY_CHUNK;
    }

    shift_addr -= 1024;
    if(shift_addr<768){
        // chunk128
        if(cursor->chunk128[shift_addr/128] != 1)
            uart_puts("ERROR! Value of Chunk is not 1\r\n");
        else{
            cursor->chunk128[shift_addr/128] = 0;
            uart_puts("Free chunk128\r\n");
        }
        goto CHECK_EMPTY_CHUNK;
    }

    shift_addr -= 768;
    // chunk256
    if(cursor->chunk256[shift_addr/256] != 1)
        uart_puts("ERROR! Value of Chunk is not 1\r\n");
    else{
        cursor->chunk256[shift_addr/256] = 0;
        uart_puts("Free chunk256\r\n");
    }

CHECK_EMPTY_CHUNK:
    cursor->free_chunk_num += 1;
    if(cursor->free_chunk_num == 89){
        _rm_chunk(cursor);
        self->val[index] = ALLOCATED_SLOT_SHIFT;
        free_memory(self, free_addr);
    }
}

void free_memory(struct _RawFrameArray *self, uint64_t free_addr){
    uint32_t index = _get_index_from_mem(self->base_addr, free_addr);

    if(self->val[index] == CHUNK_SLOT){
        _free_chunk(self, free_addr, index);
        return;
    }

    int32_t free_size = self->val[index] - ALLOCATED_SLOT_SHIFT;
    if(free_size<0){
        uart_puts("ERROR: The address is available now\r\n");
        return;
    }
    int32_t free_size_power = _cal_log_2(free_size);
    
    uint32_t cur_index;
    uint32_t free_size_power_len = 1<<free_size_power;

    if(free_size+1 == free_size_power_len){
        _insert_to_frameList(self, free_size_power, index);
        // Process val array
        self->val[index] = free_size_power;
        for(cur_index=index+1; cur_index<index+free_size_power_len; cur_index++)
            self->val[cur_index] = FREE_SLOT;

        if(PRINT_ALLOCATE_LOG){
            // Print Log
            uart_puts("- - - - - - - - - -\r\n");
            uart_puts("Free Memory Block: from idx ");
            uart_puts(itoa(index, 10));
            uart_puts(" to idx ");
            uart_puts(itoa(index+free_size_power_len-1, 10));
            uart_puts("\r\n");
        }
    }
    else if(free_size < free_size_power_len){
        uint32_t smaller_free_size_power_len = 1<<(free_size_power-1);
        _insert_to_frameList(self, free_size_power-1, index);
        // Process val array
        self->val[index] = free_size_power-1;
        for(cur_index=index+1; cur_index<index+smaller_free_size_power_len; cur_index++)
            self->val[cur_index] = FREE_SLOT;

        if(PRINT_ALLOCATE_LOG){
            // Print Log
            uart_puts("- - - - - - - - - -\r\n");
            uart_puts("Free Memory Block: from idx ");
            uart_puts(itoa(index, 10));
            uart_puts(" to idx ");
            uart_puts(itoa(index+smaller_free_size_power_len-1, 10));
            uart_puts("\r\n");
        }

        index += free_size+1;
        if(PRINT_ALLOCATE_LOG){
            uart_puts("- - - - - - - - - -\r\n");
            uart_puts("Free Residual Memory Block: from idx ");
            uart_puts(itoa(cur_index, 10));
        }
        while(cur_index < index){
            _insert_to_frameList(self, 0, cur_index);
            self->val[cur_index] = 0;
            cur_index += 1;
        }
        if(PRINT_ALLOCATE_LOG){
            uart_puts(" to idx ");
            uart_puts(itoa(index-1, 10));
            uart_puts("\r\n");
        }
    }
    else{
        uart_puts("ERROR! free_size should not greater than free_size_power_len\r\n");
        return;
    }
    _merge_free_list(self);
}

int32_t _find_suitable_slot_size(struct _RawFrameArray *self, int32_t need_size_power){
    int32_t find_size_power = need_size_power;
    while(1){
        if(find_size_power > 19){
            uart_puts("ERROR: new_memory cannot find suitable free memory!\r\n");
            return -1;
        }
        if(self->freeList[find_size_power])
            break;
        find_size_power += 1;
    }
    return find_size_power;
}

struct FrameChunk* _get_new_chunk_element_from_zero(struct _RawFrameArray *self, struct FrameChunk *prev){
    uint64_t base_addr = _allocate_slot(self, 0, 0, _find_suitable_slot_size(self, 0), 1);
    uint32_t index = _get_index_from_mem(self->base_addr, base_addr);
    return _get_new_chunk_element(index, 0, prev);
}

uint64_t _allocate_chunk(struct _RawFrameArray *self, struct FrameChunk *cursor, uint16_t chunk_size, \
                    uint8_t chunk_len, uint16_t base){
    uint8_t *chunk_cursor, free_idx;
    while(1){
        if(!cursor->free_chunk_num){
            if(cursor->next){
                cursor = cursor->next;
                continue;
            }
            else{
                cursor->next = _get_new_chunk_element_from_zero(self, cursor);
                cursor = cursor->next;
            }
        }
        free_idx = 0;
        switch(chunk_size){
            case 16:
                chunk_cursor = cursor->chunk16;
                break;
            case 32:
                chunk_cursor = cursor->chunk32;
                break;
            case 64:
                chunk_cursor = cursor->chunk64;
                break;
            case 128:
                chunk_cursor = cursor->chunk128;
                break;
            case 256:
                chunk_cursor = cursor->chunk256;
                break;
            default:
                uart_puts("ERROR: Wrong Chunk Size!\r\n");
                return -1;
        }

        while(free_idx<chunk_len && chunk_cursor[free_idx]==1)
            free_idx += 1;

        if(free_idx<chunk_len){
            chunk_cursor[free_idx] = 1;
            cursor->free_chunk_num -= 1;
            return self->base_addr + (4096 * (uint64_t)cursor->index) + base + free_idx*chunk_size;
        }
        else if(cursor->next)
            cursor = cursor->next;
        else{
            cursor->next = _get_new_chunk_element_from_zero(self, cursor);
            cursor = cursor->next;
        }
    }
}

uint64_t _new_chunk(struct _RawFrameArray *self, uint64_t need_size){
    if(!chunk_list)
        chunk_list = _get_new_chunk_element_from_zero(self, 0);

    struct FrameChunk *cursor = chunk_list;
    uint8_t need_size_pow = _cal_log_2(need_size);
    if(need_size_pow<5){
        // chunk16
        uart_puts("Allocate chunk 16\r\n");
        return _allocate_chunk(self, cursor, 16, 32, 0);
    }
    else if(need_size_pow==5){
        // chunk32
        uart_puts("Allocate chunk 32\r\n");
        return _allocate_chunk(self, cursor, 32, 32, 512);
    }
    else if(need_size_pow==6){
        // chunk64
        uart_puts("Allocate chunk 64\r\n");
        return _allocate_chunk(self, cursor, 64, 16, 1536);
    }
    else if(need_size_pow==7){
        // chunk128
        uart_puts("Allocate chunk 128\r\n");
        return _allocate_chunk(self, cursor, 128, 6, 2560);
    }
    else if(need_size_pow==8){
        // chunk256
        uart_puts("Allocate chunk 256\r\n");
        return _allocate_chunk(self, cursor, 256, 3, 3328);
    }
    else{
        uart_puts("ERROR! size should not use new_chunk function\r\n");
        return -1;
    }
}

uint64_t new_memory(struct _RawFrameArray *self, uint64_t need_size){
    if(need_size < 256)
        return _new_chunk(self, need_size);

    need_size /= 0x1000;
    int32_t need_size_power = _cal_log_2(need_size);
    int32_t find_size_power = _find_suitable_slot_size(self, need_size_power);
    return _allocate_slot(self, need_size, need_size_power, find_size_power, 0);
}

FrameArray* NewFrameArray(){
    static FrameArray frame_array;
    frame_array.base_addr = 0x12000000; // 0x10000000 ~ 0x12000000 => small chunk
    frame_array.end_addr = 0x20000000;  // 0x12000000 ~ 0x20000000 => large page
    int32_t idx = _cal_log_2(0x10000);

    frame_array.val[0] = idx;
    int i;
    for(i=1; i<0x10000; i++)
        frame_array.val[i] = FREE_SLOT;
    
    for(i=0; i<20; i++)
        frame_array.freeList[i] = 0;

    frame_array.freeList[idx] = _get_new_list_element(0, 0, 0);

    return &frame_array;
}