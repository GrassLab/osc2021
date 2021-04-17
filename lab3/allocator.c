#include "allocator.h"
#include "uart.h"
#include "string.h"
#include "util.h"

#define ALLOCATE_START      0x10000000
#define ALLOCATE_END        0x1fffffff
#define FRAME_SIZE      (4 * 1024)
#define FRAME_ARRAY_LEN     ((ALLOCATE_END - ALLOCATE_START + 1) / FRAME_SIZE)
#define MAX_BLOCK_SIZE_EXP      17  // 1 << (MAX_BLOCK_SIZE_EXP - 1) = FRAME_ARRAY_LEN

#define POOL_SIZE_LEN       (sizeof(pool_sizes) / sizeof(int))

#define MEMORY_UNIT_LIST_LEN        (sizeof(memory_unit_list) / sizeof(struct Memory_unit))

#define MAX_CHUNK_NUM       ((FRAME_SIZE / pool_sizes[0]) / 8)


/*
 * state >= 0 : There is an allocable, contiguous memory that starts from the idx’th frame with size=2^(val) × 4kb. 
 * state = -1 : The idx’th frame is free, but it belongs to a larger contiguous memory block.
 *              Hence, buddy system doesn’t directly allocate it.
 * state = -2 : The idx’th frame is already allocated, hence not allocable.
 */
static Frame frame_array[FRAME_ARRAY_LEN];
static Frame *free_block_list[MAX_BLOCK_SIZE_EXP];

static int pool_sizes[] = {16, 32, 64, 128, 256, 512};
static Frame *pool_list[POOL_SIZE_LEN];

void set_frame(Frame *frame, int state) {
    frame->state = state;
}

void set_chunk(Frame *frame, int size) {
    frame->chunk_size = size;
    frame->chunk_num = (FRAME_SIZE / size) / 8;
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

void init_blocks() {
    for (int i = 0; i < MAX_BLOCK_SIZE_EXP; i++) {
        free_block_list[i] = NULL;
    }
}

void init_frame_array() {
    for (int i = 0; i < FRAME_ARRAY_LEN; i++) {
        frame_array[i].index = i;
        frame_array[i].chunk_size = 0;
        frame_array[i].next_chunks = NULL;
        for (int j = 0; j < MAX_CHUNK_NUM; j++) {
            frame_array[i].chunks[j] &= 0;
        }
        set_frame(&frame_array[i], -1);
    }

    unsigned long frame_array_len = FRAME_ARRAY_LEN;
    int index = 0;
    for (int exp = 0; exp < MAX_BLOCK_SIZE_EXP; frame_array_len = frame_array_len >> 1, exp++) {
        if (frame_array_len % 2) {
            push_useless_block(&frame_array[index], exp);
            set_frame(&frame_array[index], exp);
            index += (1 << exp);
        }
    }
}

void init_memory() {
    uart_put_str("initial memory.\n");
    init_blocks();
    init_frame_array();
    
}

void welcome() {
    uart_put_str("start allocator.\n");
    uart_put_str("The range of memory you can manage is from ");
    uart_put_addr(ALLOCATE_START);
    uart_put_str(" to ");
    uart_put_addr(ALLOCATE_END);
    uart_put_str("\n");
}

unsigned long get_memory_unit(char *type) {
    while (1) {
        uart_put_str("[");
        uart_put_str(type);
        uart_put_str("]");
        uart_put_str("Enter memory unit(");
        for (int i = 0; i < MEMORY_UNIT_LIST_LEN; i++) {
            uart_put_str(memory_unit_list[i].name);
            uart_put_str(", ");
        }
        uart_put_str("reselect): ");
        
        char *memory_unit;
        memory_unit = uart_get_str();
        
        if (strcmp(memory_unit, "reselect"))
            return 0;
        for (int i = 0; i < MEMORY_UNIT_LIST_LEN; i++) {
            if (strcmp(memory_unit, memory_unit_list[i].name))
                return memory_unit_list[i].offset;
        }

        uart_put_str("Please enter '");
        for (int i = 0; i < MEMORY_UNIT_LIST_LEN; i++) {
            uart_put_str(memory_unit_list[i].name);
            uart_put_str("', '");
        }
        uart_put_str("reselect.'\n");
    }
}

unsigned long get_size(char *type, unsigned long unit) {
    while (1) {
        uart_put_str("[");
        uart_put_str(type);
        uart_put_str("]");
        uart_put_str("Enter memory size(1 ~ 1023 or 0 to reselect): ");
        unsigned long size = dec_str2int(uart_get_str());
        if (size > 1023) {
            uart_put_str("Please enter 1 ~ 1023 or 0.\n");
            continue;
        }

        return size * unit;
    }

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

Frame *set_useless_block(int exp) {
    if (exp == MAX_BLOCK_SIZE_EXP)
        return NULL;
    
    Frame *block = peek_useless_block(exp);
    if (!block) {
        block = set_useless_block(exp + 1);
        pop_useless_block(exp + 1);

        if (block) {
            set_frame(block, exp);
            set_frame(block + (1 << exp), exp);
            push_useless_block(block + (1 << exp), exp);
            push_useless_block(block, exp);
            
            unsigned long start_addr = ALLOCATE_START + block->index * FRAME_SIZE;
            unsigned long middle_addr = start_addr + (1 << exp) * FRAME_SIZE;
            unsigned long end_addr = middle_addr + (1 << exp) * FRAME_SIZE - 1;
            uart_put_str("==================================================================\n");
            uart_put_str("split memory from ");
            uart_put_addr(start_addr);
            uart_put_str(" to ");
            uart_put_addr(end_addr);
            uart_put_str("\none is from ");
            uart_put_addr(start_addr);
            uart_put_str(" to ");
            uart_put_addr(middle_addr - 1);
            uart_put_str("\nanother is from ");
            uart_put_addr(middle_addr);
            uart_put_str(" to ");
            uart_put_addr(end_addr);
            uart_put_str("\n");
            uart_put_str("==================================================================\n");
        }
    }
    return block;
}

void use_useless_block(int exp) {
    Frame *block = pop_useless_block(exp);
    for (int i = 0; i < (1 << exp); i++) {
        set_frame(block + i, -2);
    }

    unsigned long start_addr = ALLOCATE_START + block->index * FRAME_SIZE;
    unsigned long end_addr = start_addr + (1 << exp) * FRAME_SIZE - 1;
    uart_put_str("Allocate ");
    uart_put_int((1 << exp));
    uart_put_str(" frame, from ");
    uart_put_addr(start_addr);
    uart_put_str(" to ");
    uart_put_addr(end_addr);
    uart_put_str("\n");
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

/* take top to pair others*/
void merge_useless_blocks(int exp) {
    Frame *top_block = free_block_list[exp];
    Frame *block = top_block;
    Frame *pre_block;
    int new_page_index = block->index;
    while (block->next_block) {
        pre_block = block;
        block = block->next_block;
        if (new_page_index / (1 << (exp + 1)) == (block->index) / (1 << (exp + 1))) {
            int index = new_page_index / (1 << (exp + 1)) * (1 << (exp + 1));
            pop_useless_block(exp);
            if (pre_block == top_block)
                free_block_list[exp] = block->next_block;
            else
                pre_block->next_block = block->next_block;
            
            set_frame(&frame_array[index], exp + 1);
            set_frame(&frame_array[index + (1 << exp)], -1);
            push_useless_block(&frame_array[index], exp + 1);

            unsigned long start_addr = ALLOCATE_START + index * FRAME_SIZE;
            unsigned long middle_addr = start_addr + (1 << exp) * FRAME_SIZE;
            unsigned long end_addr = middle_addr + (1 << exp) * FRAME_SIZE - 1;
            uart_put_str("==================================================================\n");
            uart_put_str("merge blocks from ");
            uart_put_addr(start_addr);
            uart_put_str(" to ");
            uart_put_addr(end_addr);
            uart_put_str("\none is from ");
            uart_put_addr(start_addr);
            uart_put_str(" to ");
            uart_put_addr(middle_addr - 1);
            uart_put_str("\nanother is from ");
            uart_put_addr(middle_addr);
            uart_put_str(" to ");
            uart_put_addr(end_addr);
            uart_put_str("\n");
            uart_put_str("==================================================================\n");

            merge_useless_blocks(exp + 1);
            break;
        }
    }
}

void free_page(unsigned long address, unsigned long size) {
    int frame_num = (size - 1) / FRAME_SIZE + 1;
    int block_exp = check_block_exp(frame_num);
    int block_index = (address - ALLOCATE_START) / FRAME_SIZE;

    if (frame_array[block_index].state != -2) {
        uart_put_str("This page already free!\n");
        return;
    }

    set_frame(&frame_array[block_index], block_exp);
    for (int i = 1; i < frame_num; i++) {
       set_frame(&frame_array[block_index + i], -1);
    }
    
    unsigned long start_addr = ALLOCATE_START + block_index * FRAME_SIZE;
    unsigned long end_addr = start_addr + (1 << block_exp) * FRAME_SIZE - 1;
    uart_put_str("Free ");
    uart_put_int((1 << block_exp));
    uart_put_str(" frame, from ");
    uart_put_addr(start_addr);
    uart_put_str(" to ");
    uart_put_addr(end_addr);
    uart_put_str("\n");

    push_useless_block(&frame_array[block_index], block_exp);
    merge_useless_blocks(block_exp);
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
            for (int i = 0; i < frame->chunk_num; i++) {
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
    unsigned long start_addr = ALLOCATE_START + frame->index * FRAME_SIZE;
    unsigned long end_addr;
    int index = 0;
    for (int i = 0; i < frame->chunk_num; i++) {
        if (frame->chunks[i] != 0xff) {
            unsigned char x = 0x80;
            for (int j = 0; j < 8; j++) {
                if ((frame->chunks[i] & x) == 0) {
                    frame->chunks[i] += x;

                    start_addr += index * frame->chunk_size;
                    end_addr = start_addr + frame->chunk_size - 1;
                    uart_put_str("Allocate ");
                    uart_put_int(frame->chunk_size);
                    uart_put_str(" bytes, from ");
                    uart_put_addr(start_addr);
                    uart_put_str(" to ");
                    uart_put_addr(end_addr);
                    uart_put_str("\n");
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

void return_frame(int size_index) {
    Frame *frame = pool_list[size_index];
    Frame *pre_frame = pool_list[size_index];

    while (frame) {
        for (int i = 0; i < frame->chunk_num; i++) {
            if (frame->chunks[i] != 0)
                break;
            if (i == frame->chunk_num - 1) {
                free_page(ALLOCATE_START + frame->index * FRAME_SIZE, FRAME_SIZE);
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

void free_small_memory(unsigned long address, unsigned long size) {
    int frame_index = (address - ALLOCATE_START) / FRAME_SIZE;
    Frame *frame = &frame_array[frame_index];
    
    int size_index = 0;
    while (size > pool_sizes[size_index++]);
    size_index--;
    size = pool_sizes[size_index];
    if (frame->chunk_size != size) {
        uart_put_str("size dose not match.\n");
        return;
    }

    int offest = address - ALLOCATE_START - frame_index * FRAME_SIZE;
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

    unsigned long start_addr = address;
    unsigned long end_addr = start_addr + frame->chunk_size - 1;
    uart_put_str("Free ");
    uart_put_int(frame->chunk_size);
    uart_put_str(" bytes, from ");
    uart_put_addr(start_addr);
    uart_put_str(" to ");
    uart_put_addr(end_addr);
    uart_put_str("\n");

    return_frame(size_index);
}

void allocator() {
    char *actions[2] = {"Allocate", "Free"};
    welcome();

    while (1) {
        uart_put_str("------------------------------------------------------------------\n");
        uart_put_str("(1) Allocate, (2) Free, (3) quit\n");
        uart_put_str("Enter: ");
        char action = uart_get_char();
        uart_send(action);
        uart_put_str("\n");
        if (action != '1' && action != '2') {
            if (action == '3')
                return;
            uart_put_str("Please enter '1' or '2' or '3'\n");
            uart_put_str("Enter: ");
            continue;
        }
        

        unsigned long unit = get_memory_unit(actions[action - '1']);
        if (!unit)
            continue;

        unsigned long size = get_size(actions[action - '1'], unit);
        if (!size)
            continue;
        
        if (action == '1') {
            if (size > pool_sizes[POOL_SIZE_LEN - 1])
                allocate_page(size);
            else
                allocate_small_memory(size);
        }
        else if (action == '2') {
            uart_put_str("address: ");
            unsigned long address = hex_str2long(uart_get_str());
            if (address < ALLOCATE_START || address > ALLOCATE_END) {
                uart_put_str("permission deny.\n");
                continue;
            }
            if (size > pool_sizes[POOL_SIZE_LEN -1])
                free_page(address, size);
            else
                free_small_memory(address, size);
        }
        
    }

}