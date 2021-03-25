#include "buddy.h"
#include "system.h"
#include "uart.h"

memFrame frame_array[FRAME_ARRAY_LENGTH];
memFrame *free_list[MEMORY_LIST_LENGTH];
memFrame *used_list[MEMORY_LIST_LENGTH];

void buddy_init(){
    for(int i = 0; i < MEMORY_LIST_LENGTH; ++i) free_list[i] = used_list[i] = 0;
    for(int i = 0; i < FRAME_ARRAY_LENGTH; ++i) frame_array[i] = (memFrame){.size = -1, .addr = (char*)MEMORY_START + PAGE_SIZE * i, .next=0};
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
memFrame* buddy_alloc(unsigned long int size){
    int exp = __fit_size_exp(size);
    if(free_list[exp] == 0) buddy_new(exp);
    memFrame* target = free_list[exp];
    free_list[exp] = target->next;
    target->next = used_list[exp];
    target->size -= MEMORY_LIST_LENGTH;
    used_list[exp] = target;
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
    if(frame_array[buddy_idx].size < 0){
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
        frame_array[buddy_idx].size = -1;
        frame_array[frame_idx].size += 1;
        frame_array[frame_idx].next = free_list[frame_array[frame_idx].size];
        free_list[frame_array[frame_idx].size] = &frame_array[frame_idx];
        buddy_merge(frame_idx);
    }
}
void buddy_free(char* addr){
    int idx = 0;
    while(frame_array[idx].addr != addr)
        ++idx;
        
    frame_array[idx].size += MEMORY_LIST_LENGTH;
    used_list[frame_array[idx].size] = frame_array[idx].next;
    frame_array[idx].next = 0;
    buddy_merge(idx);
}

int __fit_size_exp(unsigned long int size){
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
            uart_printhex((unsigned long long)cur->addr);
            uart_send(' ');
        }
        uart_puts("\r\n");
    }
}
void buddy_test1(){
    __show_buddy_system();
    uart_puts("alloc 60KB\r\n");
    void* addr1 = malloc(60*KB);
    __show_buddy_system();
    uart_puts("alloc 128KB\r\n");
    void* addr2 = malloc(128*KB);
    __show_buddy_system();
    uart_puts("release 60KB\r\n");
    buddy_free((char*)addr1);
    __show_buddy_system();
    uart_puts("release 128KB\r\n");
    buddy_free((char*)addr2);
    __show_buddy_system();
}