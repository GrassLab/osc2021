#include "dynamic_allocator.h"

static uint32_t DMA_HEADER_SIZE;
DMA_header *mem_pool;

uint32_t __align_8byte(uint32_t size){
    // 8 bytes alignment
    return (size & 0x07) ? (size & 0xfffffff8) + 8 : size;
}
uint8_t __address_to_entry(uint64_t address){
    return (uint8_t)((address - MEMORY_START) / PAGE_SIZE);
}
uint32_t __encode_DMA_info(uint8_t entry, uint8_t block_size_exp, uint8_t is_used){
    return ((uint32_t)entry << 16) | ((uint32_t)block_size_exp + MEMORY_LIST_LENGTH << 8) | is_used;
}
uint32_t __decode_block_size(uint32_t info){
    uint32_t exp = ((info & 0x0000ff00) >> 8);
    return (1 << exp) * PAGE_SIZE;
}
uint8_t __memory_unused(uint32_t info){
    return ((info & 0x000000ff) == 0) ? 1 : 0;
}
uint32_t __decode_DMA_block(uint32_t info){
    return info & 0x00ff0000;
}
uint8_t __same_block(uint32_t info1, uint32_t info2){
    return (__decode_DMA_block(info1) == __decode_DMA_block(info2)) ? 1 : 0;
}
void __mark_memory_unused(DMA_header* ptr){
    ptr->info &= 0xffffff00;
}
uint8_t __check_memory_releasable(DMA_header* ptr){
    return (__decode_block_size(ptr->info) == ptr->size + DMA_HEADER_SIZE ) ? 1 : 0;
}
void __log_block_release(DMA_header* ptr){
    if(DEBUG & 0x02){
        uart_puts("[Block release] address: ");
        uart_printhex((uint64_t)ptr);
        uart_puts(", size: ");
        uart_printhex(ptr->size + DMA_HEADER_SIZE);
        uart_puts("\r\n");
    }
}
void __log_chunk_merge(uint64_t addr1, uint64_t addr2){
    if(DEBUG & 0x02){
        uart_puts("[Merge two chunk] address: ");
        uart_printhex(addr1); uart_puts(", "); uart_printhex(addr2);
        uart_puts("\r\n");
    }
}

void dynamic_allocator_init(){
    DMA_HEADER_SIZE = sizeof(DMA_header);
    mem_pool = nullptr;
}
void __split_chunk(DMA_header* ptr, uint32_t request_size){
    uint32_t remain_size = ptr->size - request_size;
    ptr->size = request_size;
    ptr->info |= 0x01; // marked as used
    if(remain_size < DMA_HEADER_SIZE){
        ptr->size += remain_size;
        return;
    }
    else{
        DMA_header* next_chunk = (DMA_header*)((char*)ptr + ptr->size + DMA_HEADER_SIZE);
        next_chunk->size = remain_size - DMA_HEADER_SIZE;
        next_chunk->info = ptr->info & 0xffffff00; // same entry and block exp, marked as unused
        next_chunk->next = ptr->next;
        next_chunk->prev = (char*)ptr;
        if(ptr->next != nullptr) ((DMA_header*)ptr->next)->prev = (char*)next_chunk;
        ptr->next = (char*)next_chunk;
    }
}
void* dynamic_alloc(uint32_t request_size){
    if(request_size == 0) return nullptr;
    request_size = __align_8byte(request_size);
    
    DMA_header *res = nullptr;
    uint32_t proper_size = MEMORY_END - MEMORY_START;
    for(DMA_header *cur = mem_pool; cur != nullptr; cur = (DMA_header*)cur->next){
        if(__memory_unused(cur->info) && 
            cur->size >= request_size && cur->size < proper_size){
            res = cur;
            proper_size = cur->size;
        }
    }
    if(res == nullptr){
        memFrame *block = buddy_alloc(request_size + DMA_HEADER_SIZE);
        res = (DMA_header*)(block->addr);
        res->size = (1 << (block->size + MEMORY_LIST_LENGTH)) * PAGE_SIZE - DMA_HEADER_SIZE;
        res->info = __encode_DMA_info(__address_to_entry((uint64_t)(block->addr)), block->size, 0);
        res->next = (char*)mem_pool;
        res->prev = nullptr;
        if(res->next != nullptr)
            ((DMA_header*)(res->next))->prev = (char*)res;
        mem_pool = res;
    }

    __split_chunk(res, request_size);
    return (void*)((char*)res + DMA_HEADER_SIZE);
}

uint8_t __dma_merge(DMA_header* m1, DMA_header* m2){
    if(__memory_unused(m1->info) && __memory_unused(m2->info) &&
        __same_block(m1->info, m2->info)){
        
        __log_chunk_merge((uint64_t)m1, (uint64_t)m2);
        m1->size += m2->size + DMA_HEADER_SIZE;
        m1->next = m2->next;
        if(m2->next != nullptr) ((DMA_header*)(m2->next))->prev = (char*)m1;
        return 1;
    }
    else return 0;
}
void dynamic_free(void* addr){
    DMA_header* ptr = (DMA_header*)((char*)addr - DMA_HEADER_SIZE);
    __mark_memory_unused(ptr);
    __dma_merge(ptr, (DMA_header*)ptr->next);
    if(__dma_merge((DMA_header*)ptr->prev, ptr)){
        ptr = (DMA_header*)ptr->prev;
    }
    
    if(__check_memory_releasable(ptr)){
        __log_block_release(ptr);
        if(ptr->prev != nullptr){
            ((DMA_header*)ptr->prev)->next = ptr->next;
            ((DMA_header*)ptr->next)->prev = (char*)((DMA_header*)ptr->prev);
        }
        else{
            mem_pool = (DMA_header*)ptr->next;
            ((DMA_header*)ptr->next)->prev = nullptr;
        }
        buddy_free((char*)ptr);
    }
}
void show_memory_pool(){
    for(DMA_header* ptr = mem_pool; ptr != nullptr; ptr = (DMA_header*)ptr->next){
        uart_puts("info[-, entry, exp, used]: ");
        uart_printhex(ptr->info);
        uart_puts(", chunk size = ");
        uart_printhex(ptr->size);
        uart_puts(", address: ");
        uart_printhex((uint64_t)ptr);
        uart_puts("\r\n");
    }
    uart_puts("=====================================================\r\n");
}
void DMA_test(){
    uart_puts("==================  DMA_test  =======================\r\n");
    show_memory_pool();
    uint32_t size[6] = {
        sizeof(int) * 1, 
        sizeof(int) * 2201,
        sizeof(int) * 100,
        sizeof(int) * 3068,
        sizeof(int) * 9,
        sizeof(int) * 8, // over single page;
    };
    int index[6] = {0, 5, 1, 4, 3, 2};
    void *addr[6];
    for(int i = 0; i < 6; ++i){
        uart_puts("[Allocate memory] size: ");
        uart_printhex(size[i]);
        uart_puts("\r\n");
        addr[i] = dynamic_alloc(size[i]);
        show_memory_pool();
    }
    for(int i = 0; i < 6; ++i){
        uart_puts("[Deallocate memory] address: ");
        uart_printhex((uint64_t)addr[index[i]] - DMA_HEADER_SIZE);
        uart_puts(", size: ");
        uart_printhex(size[index[i]]);
        uart_puts("\r\n");
        dynamic_free(addr[index[i]]);
        show_memory_pool();
    }
    uart_puts("================  DMA_test done  =====================\r\n");
}
void DMA_test2(){
    uart_puts("==================  DMA_test  =======================\r\n");
    show_memory_pool();
    uint32_t size[6] = {
        sizeof(int) * 1, 
        sizeof(int) * 2201,
        sizeof(int) * 100,
        sizeof(int) * 3068,
        sizeof(int) * 9,
        sizeof(int) * 8, // over single page;
    };
    int index[12] = {0, 1, 2, -6, 3, -3, -5, -4, 4, 5, -2, -1};
    void *addr[6];
    for(int i = 0; i < 12; ++i){
        if(index[i] >= 0){
            uart_puts("[Allocate memory] size: ");
            uart_printhex(size[index[i]]);
            uart_puts("\r\n");
            addr[index[i]] = dynamic_alloc(size[index[i]]);
        }
        else{
            int ti = index[i] + 6;
            uart_puts("[Deallocate memory] address: ");
            uart_printhex((uint64_t)addr[ti] - DMA_HEADER_SIZE);
            uart_puts(", size: ");
            uart_printhex(size[ti]);
            uart_puts("\r\n");
            dynamic_free(addr[ti]);
        }
        show_memory_pool();
    }
    
    uart_puts("================  DMA_test done  =====================\r\n");
}