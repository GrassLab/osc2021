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
    if(header)
        while(!header->next)
            header = header->next;
    return header;
}

struct FrameListNum* _get_new_list_element(){
    cur_element_idx += 1;
    freeListElement[cur_element_idx-1].next = 0;
    return &freeListElement[cur_element_idx-1];
}

uint32_t _allocate_slot(struct _RawFrameArray *frame_array, uint64_t need_size, int16_t need_size_power, int16_t find_size_power){
    uint32_t index = frame_array->freeList[find_size_power]->index;
    uint32_t return_addr = frame_array->base_addr + (4096 * (uint32_t)index);
    struct FrameListNum *cursor;
    frame_array->freeList[find_size_power] = frame_array->freeList[find_size_power]->next;
    uint32_t cur_end_index;
    int16_t cur_size_power;

    if(find_size_power != need_size_power){
        // Process freeList
        cur_end_index = index + (1<<find_size_power);
        cur_size_power = find_size_power - 1;

        while(cur_size_power >= need_size_power){
            cur_end_index = (cur_end_index + index)/2;
            if(!frame_array->freeList[cur_size_power]){
                frame_array->freeList[cur_size_power] = _get_new_list_element();
                frame_array->freeList[cur_size_power]->index = cur_end_index;
            }
            else{
                cursor = _get_last_list_element(frame_array->freeList[cur_size_power]);
                cursor->next = _get_new_list_element();
                cursor->next->index = cur_end_index;
            }

            // Process val array
            frame_array->val[cur_end_index] = cur_size_power;
            cur_size_power -= 1;
        }
    }

    cur_end_index = index + (1<<need_size_power);
    uint32_t cur_index = index;
    for(;cur_index<cur_end_index; cur_index++)
        frame_array->val[cur_index] = UNFREE_SLOT;

    uart_puts("- - - - - - - - - -\r\n");
    uart_puts("New Memory: from idx ");
    uart_puts(itoa(index, 10));
    uart_puts(" to idx ");
    uart_puts(itoa(cur_end_index-1, 10));
    uart_puts("\r\n");
    uart_puts("- - - - - - - - - -\r\n");

    return return_addr;
}

uint32_t new_memory(struct _RawFrameArray *self, uint64_t need_size){
    need_size /= 0x1000;
    int16_t need_size_power = _cal_log_2(need_size), find_size_power;
    find_size_power = need_size_power;
    while(1){
        if(find_size_power > 19){
            uart_puts("ERROR: new_memory cannot find suitable free memory!");
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

    frame_array.freeList[idx] = _get_new_list_element();
    frame_array.freeList[idx]->index = 0;

    frame_array.new_memory = new_memory;

    return &frame_array;
}