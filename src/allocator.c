#include "stdint.h"
#include "mini_uart.h"
#include "allocator.h"
#include "str_tool.h"
#include "allocator_utils.h"

extern FrameArray *frame_array;
extern struct FrameListNum freeListElement[];
extern struct FrameChunk freeChunkElement[];
extern uint32_t cur_element_idx;
extern uint32_t cur_chunk_idx;
extern struct FrameChunk *chunk_list;

uint64_t _new_frame(uint64_t need_size, int32_t need_size_power, int32_t find_size_power, uint8_t is_chunk){
    uint32_t index = frame_array->freeList[find_size_power]->index;
    uint64_t return_addr = frame_array->base_addr + (4096 * (uint64_t)index);

    // Use first freeList element for required memory
    frame_array->freeList[find_size_power] = frame_array->freeList[find_size_power]->next;
    if(frame_array->freeList[find_size_power])
        frame_array->freeList[find_size_power]->prev = 0;

    uint32_t cur_end_index;
    int32_t cur_size_power;

    if(find_size_power != need_size_power){
        // Process freeList
        cur_end_index = index + (1<<find_size_power);
        cur_size_power = find_size_power - 1;

        while(cur_size_power >= need_size_power){   //release right bottom half
            cur_end_index = (cur_end_index + index)/2;
            _new_frameList_element(cur_size_power, cur_end_index);      //put it back to freelist

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
    while(base){        //release redundant block
        if(redundant_block_num&base){
            cur_end_index -= base;
            // Process freeList
            _new_frameList_element(idx, cur_end_index);

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

void _free_chunk(uint64_t free_addr, uint32_t index){
    struct FrameChunk *cursor = chunk_list;
    while(cursor->index != index)       //find the correspond index of chunk_list
        cursor = cursor->next;
    uint64_t shift_addr = free_addr - frame_array->base_addr - (4096 * (uint64_t)index);        //calculate the distance from base address
    uint8_t freed=0;
    if(shift_addr<512){
        // chunk16
        if(cursor->chunk16[shift_addr/16] != 1){
            uart_puts("ERROR! Value of Chunk is not 1\r\n");
            return;
        }
        else if(!freed){
            cursor->chunk16[shift_addr/16] = 0;
            uart_puts("Free chunk16 of page ");
            uart_puts(itoa(index, 10));
            uart_puts("\r\n");
            freed = 1;
        }
        goto FREE_CHUNK;        //goto finish
    }

    shift_addr -= 512;
    if(shift_addr<1024){
        // chunk32
        if(cursor->chunk32[shift_addr/32] != 1){
            uart_puts("ERROR! Value of Chunk is not 1\r\n");
            return;
        }
        else if(!freed){
            cursor->chunk32[shift_addr/32] = 0;
            uart_puts("Free chunk32 of page ");
            uart_puts(itoa(index, 10));
            uart_puts("\r\n");
            freed = 1;
        }
        goto FREE_CHUNK;
    }

    shift_addr -= 1024;
    if(shift_addr<1024){
        // chunk64
        if(cursor->chunk64[shift_addr/64] != 1){
            uart_puts("ERROR! Value of Chunk is not 1\r\n");
            return;
        }
        else if(!freed){
            cursor->chunk64[shift_addr/64] = 0;
            uart_puts("Free chunk64 of page ");
            uart_puts(itoa(index, 10));
            uart_puts("\r\n");
            freed = 1;
        }
        goto FREE_CHUNK;
    }

    shift_addr -= 1024;
    if(shift_addr<768){
        // chunk128
        if(cursor->chunk128[shift_addr/128] != 1){
            uart_puts("ERROR! Value of Chunk is not 1\r\n");
            return;
        }
        else if(!freed){
            cursor->chunk128[shift_addr/128] = 0;
            uart_puts("Free chunk128 of page ");
            uart_puts(itoa(index, 10));
            uart_puts("\r\n");
            freed = 1;
        }
        goto FREE_CHUNK;
    }

    shift_addr -= 768;
    // chunk256
    if(cursor->chunk256[shift_addr/256] != 1){
        uart_puts("ERROR! Value of Chunk is not 1\r\n");
        return;
    }
    else if(!freed){
        cursor->chunk256[shift_addr/256] = 0;
        uart_puts("Free chunk256 of page ");
        uart_puts(itoa(index, 10));
        uart_puts("\r\n");
        freed = 1;
    }

FREE_CHUNK:
    if(freed){
        cursor->free_chunk_num += 1;
        if(cursor->free_chunk_num == 89){
            _rm_chunk(cursor);
            frame_array->val[index] = ALLOCATED_SLOT_SHIFT;
            free(free_addr);
        }
    }
}

void free(uint64_t free_addr){
    uint32_t index = _get_index_from_mem(frame_array->base_addr, free_addr);

    if(frame_array->val[index] == CHUNK_SLOT){
        _free_chunk(free_addr, index);
        return;
    }

    int32_t free_size = frame_array->val[index] - ALLOCATED_SLOT_SHIFT;
    if(free_size<0){
        uart_puts("ERROR: The address is available now\r\n");
        return;
    }
    int32_t free_size_power = _cal_bit_len(free_size);
    
    uint32_t cur_index;
    uint32_t free_size_power_len = 1<<free_size_power;

    if(free_size+1 == free_size_power_len){
        _new_frameList_element(free_size_power, index);
        // Process val array
        frame_array->val[index] = free_size_power;
        for(cur_index=index+1; cur_index<index+free_size_power_len; cur_index++)
            frame_array->val[cur_index] = FREE_SLOT;

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
        _new_frameList_element(free_size_power-1, index);
        // Process val array
        frame_array->val[index] = free_size_power-1;
        for(cur_index=index+1; cur_index<index+smaller_free_size_power_len; cur_index++)
            frame_array->val[cur_index] = FREE_SLOT;

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
            _new_frameList_element(0, cur_index);
            frame_array->val[cur_index] = 0;
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
    _merge_frameList_element();
}

uint64_t _allocate_chunk(struct FrameChunk *cursor, uint16_t chunk_size, uint8_t chunk_len, uint16_t base){
    uint8_t *chunk_cursor, free_idx;
    while(1){
        if(!cursor->free_chunk_num){
            if(cursor->next){
                cursor = cursor->next;
                continue;
            }
            else{
                cursor->next = _new_chunk_from_zero(cursor);    //add a new chunk entry to the last of the list
                cursor = cursor->next;
            }
        }
        free_idx = 0;
        switch(chunk_size){
            case 16:
                chunk_cursor = cursor->chunk16; //access chunk16 array
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

        while(free_idx<chunk_len && chunk_cursor[free_idx]==1)      //go through, ex. chunk128, to see where's the empty chunk
            free_idx += 1;

        if(free_idx<chunk_len){     //there are still emptry chunk to allocate, good, calculate the address and return
            chunk_cursor[free_idx] = 1;
            cursor->free_chunk_num -= 1;
            return frame_array->base_addr + (4096 * (uint64_t)cursor->index) + base + free_idx*chunk_size;
        }
        else if(cursor->next)       //if that block and chunksize doesn't have enough space for required size, then do again
            cursor = cursor->next;
        else{
            cursor->next = _new_chunk_from_zero(cursor);
            cursor = cursor->next;
        }
    }
}

uint64_t _new_chunk(uint64_t need_size){
    if(!chunk_list)     //init for empty chunklist
        chunk_list = _new_chunk_from_zero(0);

    struct FrameChunk *cursor = chunk_list;
    uint8_t need_size_pow = _cal_bit_len(need_size);
    if(need_size_pow<5){
        // chunk16
        uart_puts("Allocate chunk 16\r\n");
        return _allocate_chunk(cursor, 16, 32, 0);  //cursor, unit size, unit length, base address
    }
    else if(need_size_pow==5){
        // chunk32
        uart_puts("Allocate chunk 32\r\n");
        return _allocate_chunk(cursor, 32, 32, 512);
    }
    else if(need_size_pow==6){
        // chunk64
        uart_puts("Allocate chunk 64\r\n");
        return _allocate_chunk(cursor, 64, 16, 1536);
    }
    else if(need_size_pow==7){
        // chunk128
        uart_puts("Allocate chunk 128\r\n");
        return _allocate_chunk(cursor, 128, 6, 2560);
    }
    else if(need_size_pow==8){
        // chunk256
        uart_puts("Allocate chunk 256\r\n");
        return _allocate_chunk(cursor, 256, 3, 3328);
    }
    else{
        uart_puts("ERROR! size should not use new_chunk function\r\n");
        return -1;
    }
}

uint64_t kmalloc(uint64_t need_size){
    if(need_size < 256)
        return _new_chunk(need_size);       //if less than 256k, then only ask for ckunk

    need_size /= 0x1000;
    int32_t need_size_power = _cal_bit_len(need_size);
    int32_t find_size_power = _find_capable_slot_size(need_size_power);
    return _new_frame(need_size, need_size_power, find_size_power, 0);      //not ask for chunk
}

FrameArray* NewFrameArray(){
    static FrameArray frame_array;
    frame_array.base_addr = 0x12000000; // 0x10000000 ~ 0x12000000 => small chunk
    frame_array.end_addr = 0x20000000;  // 0x12000000 ~ 0x20000000 => large page
    int32_t idx = _cal_bit_len(0x10000);

    frame_array.val[0] = idx;
    int i;
    for(i=1; i<0x10000; i++)
        frame_array.val[i] = FREE_SLOT;
    
    for(i=0; i<20; i++)
        frame_array.freeList[i] = 0;

    frame_array.freeList[idx] = _new_list_element(0, 0, 0);

    return &frame_array;
}