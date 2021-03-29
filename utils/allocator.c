#include "stdint.h"
#include "mini_uart.h"
#include "allocator.h"
#include "str_tool.h"

struct FrameListNum freeListElement[MAX_ELEMENT_NUM];
uint32_t cur_element_idx = 0;

int16_t _cal_log_2(uint64_t num){
    int16_t res = 0;
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

struct FrameListNum* _get_new_list_element(uint32_t index, struct FrameListNum *next, struct FrameListNum *prev){
    cur_element_idx += 1;
    freeListElement[cur_element_idx-1].index = index;
    freeListElement[cur_element_idx-1].next = next;
    freeListElement[cur_element_idx-1].prev = prev;
    return &freeListElement[cur_element_idx-1];
}

void _insert_to_frameList(struct _RawFrameArray *frame_array, uint16_t power_idx, uint32_t element_idx){
    struct FrameListNum *cursor;
    if(!frame_array->freeList[power_idx]){
        frame_array->freeList[power_idx] = _get_new_list_element(element_idx, 0, 0);
    }
    else{
        cursor = _get_last_list_element(frame_array->freeList[power_idx]);
        cursor->next = _get_new_list_element(element_idx, 0, cursor);
    }
}

uint64_t _allocate_slot(struct _RawFrameArray *frame_array, uint64_t need_size, int16_t need_size_power, int16_t find_size_power){
    uint32_t index = frame_array->freeList[find_size_power]->index;
    uint64_t return_addr = frame_array->base_addr + (4096 * (uint64_t)index);
    frame_array->freeList[find_size_power] = frame_array->freeList[find_size_power]->next;
    uint32_t cur_end_index;
    int16_t cur_size_power;

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
    
    for(;cur_index<cur_end_index; cur_index++)
        frame_array->val[cur_index] = UNFREE_SLOT;

    // Print Log
    uart_puts("- - - - - - - - - -\r\n");
    uart_puts("New Memory: from idx ");
    uart_puts(itoa(index, 10));
    uart_puts(" to idx ");
    uart_puts(itoa(cur_end_index-1, 10));
    uart_puts("\r\n");

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

            // Print Log
            uart_puts("- - - - - - - - - -\r\n");
            uart_puts("Free Redundant Block: from idx ");
            uart_puts(itoa(cur_end_index, 10));
            uart_puts(" to idx ");
            uart_puts(itoa(cur_end_index+base-1, 10));
            uart_puts("\r\n");
            redundant_block_num ^= base;
        }
        base >>= 1;
        idx -= 1;
    }
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
                    uart_puts("- - - - - - - - - -\r\n");
                    uart_puts("Merge Memory Block: from idx ");
                    uart_puts(itoa(min_val, 10));
                    uart_puts(" to idx ");
                    uart_puts(itoa(min_val+cur_base-1, 10));
                    uart_puts("\r\n");
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

void free_memory(struct _RawFrameArray *self, uint64_t free_addr, uint64_t free_size){
    free_size /= 0x1000;
    int16_t free_size_power = _cal_log_2(free_size);
    uint32_t index = (free_addr - self->base_addr) >> 12;   // divide by 4096
    uint32_t cur_index;
    uint32_t free_size_power_len = 1<<free_size_power;

    if(free_size+1 == free_size_power_len){
        _insert_to_frameList(self, free_size_power, index);
        // Process val array
        self->val[index] = free_size_power;
        for(cur_index=index+1; cur_index<index+free_size_power_len; cur_index++)
            self->val[cur_index] = FREE_SLOT;

        // Print Log
        uart_puts("- - - - - - - - - -\r\n");
        uart_puts("Free Memory Block: from idx ");
        uart_puts(itoa(index, 10));
        uart_puts(" to idx ");
        uart_puts(itoa(index+free_size_power_len-1, 10));
        uart_puts("\r\n");
    }
    else if(free_size < free_size_power_len){
        uint32_t smaller_free_size_power_len = 1<<(free_size_power-1);
        _insert_to_frameList(self, free_size_power-1, index);
        // Process val array
        self->val[index] = free_size_power-1;
        for(cur_index=index+1; cur_index<index+smaller_free_size_power_len; cur_index++)
            self->val[cur_index] = FREE_SLOT;

        // Print Log
        uart_puts("- - - - - - - - - -\r\n");
        uart_puts("Free Memory Block: from idx ");
        uart_puts(itoa(index, 10));
        uart_puts(" to idx ");
        uart_puts(itoa(index+smaller_free_size_power_len-1, 10));
        uart_puts("\r\n");

        index += free_size+1;
        uart_puts("- - - - - - - - - -\r\n");
        uart_puts("Free Residual Memory Block: from idx ");
        uart_puts(itoa(cur_index, 10));
        while(cur_index < index){
            _insert_to_frameList(self, 0, cur_index);
            self->val[cur_index] = 0;
            cur_index += 1;
        }
        uart_puts(" to idx ");
        uart_puts(itoa(index-1, 10));
        uart_puts("\r\n");
    }
    else{
        uart_puts("ERROR! free_size should not greater than free_size_power_len\r\n");
        return;
    }
    _merge_free_list(self);
}

uint64_t new_memory(struct _RawFrameArray *self, uint64_t need_size){
    need_size /= 0x1000;
    int16_t need_size_power = _cal_log_2(need_size), find_size_power;
    find_size_power = need_size_power;
    while(1){
        if(find_size_power > 19){
            uart_puts("ERROR: new_memory cannot find suitable free memory!\r\n");
            return -1;
        }
        if(self->freeList[find_size_power])
            break;
        find_size_power += 1;
    }
    return _allocate_slot(self, need_size, need_size_power, find_size_power);
}

FrameArray* NewFrameArray(){
    static FrameArray frame_array;
    frame_array.base_addr = 0x10000000;
    frame_array.end_addr = 0x20000000;
    int16_t idx = _cal_log_2(0x10000);

    frame_array.val[0] = idx;
    int i;
    for(i=1; i<0x10000; i++)
        frame_array.val[i] = FREE_SLOT;
    
    for(i=0; i<20; i++)
        frame_array.freeList[i] = 0;

    frame_array.freeList[idx] = _get_new_list_element(0, 0, 0);

    return &frame_array;
}