#include "allocator.h"
#include "uart.h"
#include "string.h"
#include "util.h"

#define RECORD_DYNAMIC_BLOCK_ADDR     0x100000
#define RECORD_DYNAMIC_BLOCK_SIZE     (1 * 1024 * 1024)
#define MEM_BLOCKS_NUM      10

#define MAX_BLOCK_SIZE_EXP      19  // 1 << (MAX_BLOCK_SIZE_EXP - 1) = Physical Memory size / FRAME_SIZE

#define POOL_SIZE_LEN       (sizeof(pool_sizes) / sizeof(int))
#define MAX_CHUNK_NUM       ((FRAME_SIZE / pool_sizes[0]) / 8)

static Memblock reserved_block[MEM_BLOCKS_NUM];
static Memblock dynamic_block[MEM_BLOCKS_NUM];

static Frame *free_block_list[MAX_BLOCK_SIZE_EXP];

static int pool_sizes[] = {16, 32, 64, 128, 256, 512};
static Frame *pool_list[POOL_SIZE_LEN];

void set_frame(Frame *frame, int state) {
    frame->state = state;
}

void set_chunk(Frame *frame, int size) {
    frame->chunk_size = size;
}

void push_useless_block(Frame *block, int exp) {
    block->next_block = free_block_list[exp];
    free_block_list[exp] = block;
}

Frame *pop_useless_block(int exp) {
    Frame *block = free_block_list[exp];
    if (block)
        free_block_list[exp] = block->next_block;
    return block;
}

Frame *peek_useless_block(int exp) {
    return free_block_list[exp];
}

int get_chunk_num(int chunk_size) {
    return (FRAME_SIZE / chunk_size) / 8;
}

void init_mem_blocks() {
    for (int i = 0; i < MEM_BLOCKS_NUM; i++) {
        reserved_block[i].size = 0;
        dynamic_block[i].size = 0;
    }
}

void init_block_list() {
    for (int i = 0; i < MAX_BLOCK_SIZE_EXP; i++) {
        free_block_list[i] = NULL;
    }
}

void set_reserved_mem(unsigned long start_addr, long size) {
    int index = 0;
    while (reserved_block[index].size  != 0) {
        index++;
        if (index >= MEM_BLOCKS_NUM)
            break;
    }

    if (index >= MEM_BLOCKS_NUM) {
        uart_put_str("set_reserverd_mem fail. addr: ");
        uart_put_addr(start_addr);
        uart_put_str("\n");
    }
    else {
        reserved_block[index].start_addr = start_addr;
        reserved_block[index].size = size;
        uart_put_str("start: ");
        uart_put_addr(start_addr);
        uart_put_str(", end: ");
        uart_put_addr(start_addr + size - 1);
        uart_put_str(", size: ");
        uart_put_ulong(size);
        uart_put_str(" Bytes\n");
    }
}

void set_dynamic_mem(unsigned long start_addr, long size) {
    int index = 0;
    while (dynamic_block[index].size != 0) {
        index++;
        if (index >= MEM_BLOCKS_NUM)
            break;
    }

    if (index >= MEM_BLOCKS_NUM) {
        uart_put_str("set_dynamic_mem fail. addr: ");
        uart_put_addr(start_addr);
        uart_put_str("\n");
    }
    else {
        dynamic_block[index].start_addr = start_addr;
        dynamic_block[index].size = size;
        uart_put_str("start: ");
        uart_put_addr(start_addr);
        uart_put_str(", end: ");
        uart_put_addr(start_addr + size - 1);
        uart_put_str(", size: ");
        uart_put_ulong(size);
        uart_put_str(" Bytes\n");
    }
}

/*
 * start allocator init
 */
void init_reserved_mem() {
    uart_put_str("[set reserved block]\n");
    set_reserved_mem(0x00000, 0xb0000);
    set_reserved_mem(RECORD_DYNAMIC_BLOCK_ADDR, RECORD_DYNAMIC_BLOCK_SIZE);
}

void init_dynamic_mem() {
    uart_put_str("[set dynamic block]\n");
    set_dynamic_mem(0x20000000, FRAME_SIZE);
    set_dynamic_mem(0x10000000, 1 * 1024 * 1024);
}

void init_frame_array() {
    Frame *record_mem = (Frame *)RECORD_DYNAMIC_BLOCK_ADDR;
    for (int i = 0; i < MEM_BLOCKS_NUM  && dynamic_block[i].size != 0; i++) {
        dynamic_block[i].record_addr = (unsigned long)record_mem;
        for (int index = 0; index < dynamic_block[i].size / FRAME_SIZE; index++, record_mem++) {
            record_mem->index = index;
            record_mem->dynamic_block_index = i;
            record_mem->chunk_size = 0;
            record_mem->next_chunks = NULL;
            for (int j = 0; j < MAX_CHUNK_NUM; j++) {
                record_mem->chunks[j] &= 0;
            }
            set_frame(record_mem, -1);
        }

        Frame *frame_array = (Frame *)dynamic_block[i].record_addr;
        unsigned long frame_array_len = dynamic_block[i].size / FRAME_SIZE;
        for (int exp = 0; exp < MAX_BLOCK_SIZE_EXP; frame_array_len = frame_array_len >> 1, exp++) {
            if (frame_array_len % 2) {
                push_useless_block(frame_array, exp);
                set_frame(frame_array, exp);
                frame_array += (1 << exp);
            }
        }
    }
    
    for (; (unsigned long)record_mem < RECORD_DYNAMIC_BLOCK_ADDR + RECORD_DYNAMIC_BLOCK_SIZE; record_mem++) {
        record_mem->index = -1;
        set_frame(record_mem, -3);
    }

}

void init_memory() {
    
    init_mem_blocks();
    init_block_list();
    init_reserved_mem();
    init_dynamic_mem();

    init_frame_array();
    
}

int check_block_exp(int block_num) {
    int num = 1, exp = 0;
    while (1) {
        if (num >= block_num)
            return exp;
        num *= 2;
        exp ++;
    }
}

int which_dynamic_block_index_by_record_addr(unsigned long record_addr) {
    int index = 0;
    for (; index < MEM_BLOCKS_NUM; index++) {
        if (dynamic_block[index].size == 0)
            break;
        if (record_addr < dynamic_block[index].record_addr)
            return index - 1;
    }
    return index - 1;
}

Frame *set_useless_block(int exp) {
    if (exp == MAX_BLOCK_SIZE_EXP)
        return NULL;
    
    Frame *block = peek_useless_block(exp);
    if (!block) {
        block = set_useless_block(exp + 1);

        if (block) {
            pop_useless_block(exp + 1);
            set_frame(block, exp);
            set_frame(block + (1 << exp), exp);
            push_useless_block(block + (1 << exp), exp);
            push_useless_block(block, exp);
        }
    }
    return block;
}

void use_useless_block(int exp) {
    Frame *block = pop_useless_block(exp);
    for (int i = 0; i < (1 << exp); i++) {
        set_frame(block + i, -2);
    }
}

Frame *allocate_page(unsigned long size) {
    int block_num = (size - 1) / FRAME_SIZE + 1;
    int block_exp = check_block_exp(block_num);
    
    Frame *block =  set_useless_block(block_exp);
    if (block)
        use_useless_block(block_exp);
    else
        uart_put_str("Out of memory!\n");
    
    return block;
}

unsigned long page_allocator(unsigned long size) {
    Frame *block = allocate_page(size);
    return dynamic_block[block->dynamic_block_index].start_addr + block->index * FRAME_SIZE;
}

/* take top to pair others*/
void merge_useless_blocks(int exp, Frame *frame_array) {
    Frame *top_block = free_block_list[exp];
    Frame *block = top_block;
    Frame *pre_block;
    while (block->next_block) {
        pre_block = block;
        block = block->next_block;
        if ( top_block->dynamic_block_index == block->dynamic_block_index &&
            (top_block->index / (1 << (exp + 1)) == (block->index) / (1 << (exp + 1)))) {
            int index = top_block->index / (1 << (exp + 1)) * (1 << (exp + 1));
            pop_useless_block(exp);
            if (pre_block == top_block)
                free_block_list[exp] = block->next_block;
            else
                pre_block->next_block = block->next_block;
            
            set_frame(&frame_array[index], exp + 1);
            set_frame(&frame_array[index + (1 << exp)], -1);
            push_useless_block(&frame_array[index], exp + 1);

            merge_useless_blocks(exp + 1, frame_array);
            break;
        }
    }
}

void free_page(unsigned long address, unsigned long size, Frame *frame_array) {
    int frame_num = (size - 1) / FRAME_SIZE + 1;
    int block_exp = check_block_exp(frame_num);
    int dynamic_block_addr = which_dynamic_block_index_by_record_addr((unsigned long)frame_array);
    int block_index = (address - dynamic_block[dynamic_block_addr].start_addr) / FRAME_SIZE;

    for (int i = 0; i < frame_num; i++) {
        if (frame_array[block_index + i].state != -2) {
            uart_put_str("some or all memory already free!\n");
            return;
        }
    }

    set_frame(&frame_array[block_index], block_exp);
    for (int i = 1; i < frame_num; i++) {
       set_frame(&frame_array[block_index + i], -1);
    }

    push_useless_block(&frame_array[block_index], block_exp);
    merge_useless_blocks(block_exp, frame_array);
}

Frame *request_frame(int size_index) {
    Frame *frame = allocate_page(FRAME_SIZE);
    if (!frame)
        return frame;

    set_chunk(frame, pool_sizes[size_index]);
    frame->next_chunks = pool_list[size_index];
    pool_list[size_index] = frame;
    return frame;
}

Frame *search_useless_chunk(int size_index) {
    if (pool_list[size_index]) {
        Frame *frame = pool_list[size_index];
        while (frame) {
            int chunk_num = get_chunk_num(frame->chunk_size);
            for (int i = 0; i < chunk_num; i++) {
                if (frame->chunks[i] != 0xff)
                    return frame;
            }
            frame = frame->next_chunks;
        }
        return request_frame(size_index);
    }
    else {
        return request_frame(size_index);
    }
}

unsigned long use_useless_chunk(Frame *frame) {
    int dynamic_block_addr = which_dynamic_block_index_by_record_addr((unsigned long)frame);
    unsigned long start_addr = dynamic_block[dynamic_block_addr].start_addr + frame->index * FRAME_SIZE;
    int chunk_num = get_chunk_num(frame->chunk_size);

    int index = 0;
    for (int i = 0; i < chunk_num; i++) {
        if (frame->chunks[i] != 0xff) {
            unsigned char x = 0x80;
            for (int j = 0; j < 8; j++) {
                if ((frame->chunks[i] & x) == 0) {
                    frame->chunks[i] += x;

                    start_addr += index * frame->chunk_size;
                    return start_addr;
                }
                x = x >> 1;
                index++;
            }
        }
        else {
            index += 8;
        }
    }
    return 0;
}

unsigned long allocate_small_memory(unsigned long size) {
    int size_index = 0;
    while (size > pool_sizes[size_index++]);
    size_index--;

    Frame *frame = search_useless_chunk(size_index);
    if (frame)
        return use_useless_chunk(frame);
    else
        return 0;

}

void return_frame(int size_index, Frame *frame_array) {
    Frame *frame = pool_list[size_index];
    Frame *pre_frame = pool_list[size_index];

    while (frame) {
        int chunk_num = get_chunk_num(frame->chunk_size);
        for (int i = 0; i < chunk_num; i++) {
            if (frame->chunks[i] != 0)
                break;
            if (i == chunk_num - 1) {
                int dynamic_block_addr = which_dynamic_block_index_by_record_addr((unsigned long)frame_array);
                free_page(dynamic_block[dynamic_block_addr].start_addr + frame->index * FRAME_SIZE, FRAME_SIZE, frame_array);
                if (pre_frame == frame)
                    pool_list[size_index] = frame->next_chunks;
                else
                    pre_frame->next_chunks = frame->next_chunks;
                
                return;
            }
        }
        pre_frame = frame;
        frame = frame->next_chunks;
    }
}

void free_small_memory(unsigned long address, unsigned long size, Frame *frame_array) {
    int dynamic_block_addr = which_dynamic_block_index_by_record_addr((unsigned long)frame_array);
    int frame_index = (address - dynamic_block[dynamic_block_addr].start_addr) / FRAME_SIZE;
    Frame *frame = &frame_array[frame_index];
    
    int size_index = 0;
    while (size > pool_sizes[size_index++]);
    size_index--;
    size = pool_sizes[size_index];
    if (frame->chunk_size != size) {
        uart_put_str("size dose not match.\n");
        return;
    }

    int offest = address - dynamic_block[dynamic_block_addr].start_addr - frame_index * FRAME_SIZE;
    int chunk_index = offest / frame->chunk_size;
    int x = 0x80;
    for (int i = chunk_index % 8; i > 0; i--) {
        x = x >> 1;
    }
    if ((frame->chunks[chunk_index / 8] & x) == 0) {
        uart_put_str("This address already free!\n");
        return;
    }
    else {
        frame->chunks[chunk_index / 8] -= x;
    }

    return_frame(size_index, frame_array);
}

int which_dynamic_block_by_start_addr(unsigned long address) {
    for (int i = 0; i < MEM_BLOCKS_NUM; i++) {
        if (address >= dynamic_block[i].start_addr && address < dynamic_block[i].start_addr + dynamic_block[i].size)
            return i;
    }
    return -1;
}

unsigned long get_memory(unsigned long size) {
    unsigned long address;
    if (size > pool_sizes[POOL_SIZE_LEN - 1])
        address = page_allocator(size);
    else
        address = allocate_small_memory(size);

    return address;
}

void free_memory(unsigned long size, unsigned long address) {
    int dynamic_block_index = which_dynamic_block_by_start_addr(address);
    Frame *frame_array = (Frame *)dynamic_block[dynamic_block_index].record_addr;
    if (dynamic_block_index < 0) {
        uart_put_str("permission deny.\n");
        return;
    }
    if (size > pool_sizes[POOL_SIZE_LEN -1])
        free_page(address, size, frame_array);
    else
        free_small_memory(address, size, frame_array);
}