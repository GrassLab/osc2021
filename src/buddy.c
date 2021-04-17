#include "buddy.h"
#include "system.h"
#include "uart.h"

memFrame frame_array[FRAME_ARRAY_LENGTH];
memFrame *free_list[MEMORY_LIST_LENGTH];
memFrame *used_list[MEMORY_LIST_LENGTH];



int __address_to_frame_array_entry(char *addr){
    return ((uint64_t)addr - MEMORY_START) / PAGE_SIZE ;
}
void __log_buddy_release(int idx){
    if(DEBUG & 0x01){
        uart_puts("[Release memory block] Address: ");
        uart_printhex((uint64_t)frame_array[idx].addr);
        uart_puts(", size: ");
        uart_printhex((uint64_t)(1 << frame_array[idx].size) * PAGE_SIZE);
        uart_puts("\r\n");
    }
}

void __log_buddy_alloc(memFrame *frame){
    if(DEBUG & 0x01){
        uart_puts("[Allocate memory block] Address: ");
        uart_printhex((uint64_t)frame->addr);
        uart_puts(", freelist index: ");
        uart_printint((frame->size)+MEMORY_LIST_LENGTH);
        uart_puts("\r\n");
    }
}
void __log_buddy_merge(int buddy_idx, int frame_idx){
    if(DEBUG & 0x01){
        uart_puts("[Merge buddy] ");
        uart_printint(buddy_idx); uart_puts(", ");
        uart_printint(frame_idx); uart_puts("\r\n");
    }
}
void buddy_init(){
    for(int i = 0; i < MEMORY_LIST_LENGTH; ++i) free_list[i] = used_list[i] = 0;
    for(int i = 0; i < FRAME_ARRAY_LENGTH; ++i) frame_array[i] = (memFrame){.size = -1, .addr = (char*)MEMORY_START + PAGE_SIZE * i, .next=nullptr};
    long int remain_mem_exp = FRAME_ARRAY_LENGTH, cur_idx = FRAME_ARRAY_LENGTH - 1;
    for(int i = MEMORY_LIST_LENGTH - 1; i >= 0 && cur_idx >= 0; --i){
        if(remain_mem_exp >> i & 1){
            free_list[i] = &frame_array[cur_idx - (1 << i) + 1];
            free_list[i]->size = i;
            cur_idx -= (1 << i);
        }
    } 
}

void buddy_new(int exp){
    if(free_list[exp + 1] == 0) buddy_new(exp + 1);
    memFrame* tmp = free_list[exp + 1];
    free_list[exp + 1] = tmp->next;
    tmp->next = 0;
    memFrame* split = tmp + (1 << exp);
    tmp->size = exp;
    split->size = exp;
    tmp->next = split;
    split->next = free_list[exp];
    free_list[exp] = tmp;
}
memFrame* buddy_alloc(uint32_t size){
    int exp = __fit_size_exp(size);
    if(free_list[exp] == 0) buddy_new(exp);
    memFrame* target = free_list[exp];
    free_list[exp] = target->next;
    target->next = used_list[exp];
    target->size -= MEMORY_LIST_LENGTH;
    used_list[exp] = target;
    __log_buddy_alloc(target);
    return target;
}
void __release_block(memFrame* target){
    memFrame* cur = free_list[target->size];
    memFrame* prev = 0;
    while(cur != target && cur != 0){
        prev = cur;
        cur = cur->next;
    }
    if(cur == 0) return;
    if(prev == 0)
        free_list[target->size] = target->next;
    else
        prev->next = target->next;
    target->next = 0;
}
void buddy_merge(int frame_idx){
    int buddy_idx = frame_idx ^ (1 << frame_array[frame_idx].size);
    if( frame_array[buddy_idx].size < 0 || 
        frame_array[buddy_idx].size != frame_array[frame_idx].size){
        // insert to free_list
         __release_block(&frame_array[frame_idx]);
        int list_idx = frame_array[frame_idx].size;
        frame_array[frame_idx].next = free_list[list_idx];
        free_list[list_idx] = &frame_array[frame_idx];
    }
    else if(frame_array[buddy_idx].size == frame_array[frame_idx].size){
        __release_block(&frame_array[buddy_idx]);
        __release_block(&frame_array[frame_idx]);
        if(buddy_idx < frame_idx) swap(&buddy_idx, &frame_idx);
        __log_buddy_merge(frame_idx, buddy_idx);
        frame_array[buddy_idx].size = -1;
        frame_array[frame_idx].size += 1;
        frame_array[frame_idx].next = free_list[frame_array[frame_idx].size];
        free_list[frame_array[frame_idx].size] = &frame_array[frame_idx];
        buddy_merge(frame_idx);
    }
}
void buddy_free(char* addr){
    int idx = 0;
    while(frame_array[idx].addr != addr && idx < FRAME_ARRAY_LENGTH)
        ++idx;
    if(idx >= FRAME_ARRAY_LENGTH) return;
    frame_array[idx].size += MEMORY_LIST_LENGTH;
    used_list[frame_array[idx].size] = frame_array[idx].next;
    frame_array[idx].next = 0;
    __log_buddy_release(idx);
    buddy_merge(idx);
}

int __fit_size_exp(uint32_t size){
    int res = 0;
    while((1 << res) * PAGE_SIZE < size) 
        ++res;
    return res;
}
void __show_buddy_system(){
    for(int i = 0; i < MEMORY_LIST_LENGTH; ++i){
        uart_printint(i);
        uart_puts(" :");
        for(memFrame* cur = free_list[i]; cur != 0; cur = cur->next){
            int entry = __address_to_frame_array_entry(cur->addr);
            uart_send('['); uart_printint(entry); uart_send(']');
            uart_printhex((unsigned long long)cur->addr);
            uart_send(' ');
        }
        uart_puts("\r\n");
    }
    uart_puts("=====================================================\r\n");
}
void buddy_test1(){
    uart_puts("==================  Buddy_system_test  =======================\r\n");
    __show_buddy_system();
    uint32_t size[6] = {
        PAGE_SIZE * 1, 
        PAGE_SIZE * 13,
        PAGE_SIZE * 10,
        PAGE_SIZE * 2,
        PAGE_SIZE * 4,
        PAGE_SIZE * 8
    };
    int index[6] = {0, 5, 1, 4, 3, 2};
    void *addr[6];
    
    for(int i = 0; i < 6; ++i){
        uart_puts("[Allocate memory] Size: ");
        uart_printhex(size[i]);
        uart_puts("\r\n");
        addr[i] = (void*)(buddy_alloc(size[i])->addr);
        __show_buddy_system();
    }
    for(int i = 0; i < 6; ++i){
        uart_puts("[Deallocate memory] Size: ");
        uart_printhex(size[index[i]]);
        uart_puts(", Address: ");
        uart_printhex((uint64_t)addr[index[i]]);
        uart_puts("\r\n");
        buddy_free((char*)addr[index[i]]);
        __show_buddy_system();
    }
    uart_puts("==============  Buddy_system_test done  ===================\r\n");
}
void buddy_test2(){
    uart_puts("==================  Buddy_system_test  =======================\r\n");
    __show_buddy_system();
    uint32_t size[6] = {
        PAGE_SIZE * 1, 
        PAGE_SIZE * 1,
        PAGE_SIZE * 10,
        PAGE_SIZE * 100,
        PAGE_SIZE * 4,
        PAGE_SIZE * 8
    };
    int index[12] = {0, 1, 2, -6, 3, -3, -5, -4, 4, 5, -2, -1};
    void *addr[6];
    
    for(int i = 0; i < 12; ++i){
        if(index[i] >= 0){
            uart_puts("[Allocate memory] Size: ");
            uart_printhex(size[i]);
            uart_puts("\r\n");
            addr[index[i]] = (void*)(buddy_alloc(size[i])->addr);
            
        }
        else{
            int ti = index[i] + 6;
            uart_puts("[Deallocate memory] Size: ");
            uart_printhex(size[ti]);
            uart_puts(", Address: ");
            uart_printhex((uint64_t)addr[ti]);
            uart_puts("\r\n");
            buddy_free((char*)addr[ti]);
        }
        __show_buddy_system();
    }
    uart_puts("==============  Buddy_system_test done  ===================\r\n");
}