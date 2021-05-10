#include "utils.h"
#include "memory_utils.h"
#include "uart.h"

#define NULL                ((void *)0)
#define MAX_MEMORY          (unsigned long)0x20010000
#define MIN_MEMORY          (unsigned long)0x20000000
#define FRAME_SIZE          (unsigned long)0x1000
#define BLOCK_STRUCT_ADDR   ((unsigned long*) 0x10000000)
#define BUDDY_STRUCT_ADDR   ((unsigned long*) 0x14000000)
#define BUCKET_STRUCT_ADDR  ((unsigned long*) 0x18000000)
#define CHUNK_STRUCT_ADDR   ((unsigned long*) 0x1C000000)
#define MIN_CHUNK_SIZE      16

/*
--------------------------------
--------------------------------
------Page Frame Allocator------
--------------------------------
--------------------------------
*/
typedef struct block_ {
    /*
    source: http://bitsquid.blogspot.com/2015/08/allocation-adventures-3-buddy-allocator.html
    Block indices
        ----------------------------------------------------------------- 
    512 |                               0                               |  level 0
        -----------------------------------------------------------------
    256 |               1               |               2               |  level 1
        -----------------------------------------------------------------
    128 |       3       |       4       |       5       |       6       |  level 2
        -----------------------------------------------------------------
    64  |   7   |   8   |   9   |  10   |  11   |  12   |  13   |  14   |  level 3
        -----------------------------------------------------------------
    32  |15 |16 |17 |18 |19 |20 |21 |22 |23 |24 |25 |26 |27 |28 |29 |30 |  level 4
        -----------------------------------------------------------------

    valid = 1: available
    valid = 0: not available
    valid = -1: allocated
    */
    int index;
    int valid;
    struct block_ *next;
    struct block_ *previous;
    struct block_ *allocated_chain;
} Block;

typedef struct buddy_ {
    struct block_ *head;
} Buddy;

void frame_init() {
    unsigned long interval = (MAX_MEMORY - MIN_MEMORY) / FRAME_SIZE;
    int power = WhichPowerOfTwo(interval);
    if (power==-1) {
        uart_puts("Wrong memory interval!!!!!\n");
        return;
    }
    else if ((MAX_MEMORY - MIN_MEMORY) % FRAME_SIZE) {
        uart_puts("Wrong memory interval or frame size!!!!!\n");
        return;
    }
    unsigned long total_frame = pow(2, power);
    unsigned long total_index = pow(2, power+1) - 1;
    Buddy *buddy = BUDDY_STRUCT_ADDR;
    Block *blocks = BLOCK_STRUCT_ADDR;

    char str[20]; // for itoa
    memset(str, 0, 20);
    
    /*
    str[0] = '0';
    str[1] = '1';
    str[2] = '\0';
    str[3] = '2';
    str[4] = '3';
    str[5] = '\0';
    uart_puts(str);
    */

    // initialize Blocks
    for (int i = 0; i < total_index; i++) {
        blocks[i].index = i;
        blocks[i].valid = 0;

        blocks[i].next = NULL;
        blocks[i].previous = NULL;
        blocks[i].allocated_chain = NULL;
    }
    blocks[0].valid = 1;

    // initialize buddy
    for (int i = 0; i <= power; i++) {
        if (!i) buddy[i].head = blocks;
        else {
            buddy[i].head = NULL;
            if (buddy[i].head) uart_puts("BUGGGGGGGGGGGGGG\n");
        }
    }

    uart_puts("Initialize page frame allocator complete!\n");
    uart_puts("Frame size: ");
    itoa(FRAME_SIZE, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts("B\n");
    uart_puts("Total available frame: ");
    itoa(total_frame, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts("\n");
    uart_puts("Total block: ");
    itoa(total_index, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts("\n");
}

int compute_level(unsigned long bytes) {
    /*
    compute which level of block we need given n bytes
    */
    unsigned long interval = (MAX_MEMORY - MIN_MEMORY) / FRAME_SIZE;
    int total_level = WhichPowerOfTwo(interval);

    unsigned long required_frame = frame_ceil(FRAME_SIZE, bytes);
    int res = WhichPowerOfTwo(required_frame);
    if (res == -1) {
        int most_significant = 32 - __builtin_clz(required_frame);
        return total_level - most_significant;
    }
    else return total_level - res;
}

int compute_level_with_index(int index) {
    int level = 0;
    int min_index = 0;
    int max_index = 0;
    while (1) {
        min_index = pow(2, level) - 1;
        max_index = min_index + pow(2, level) - 1;
        if (index >= min_index && index <= max_index) break;
        else level++;
    }
    return level;
}

void *compute_addr(int level, int index) {
    if (!index) return (void *)MIN_MEMORY;

    // number of frame at level n
    unsigned long num_frame = pow(2, level);
    
    // start index of the frame at level n
    int start = pow(2, level) - 1;

    // compute offset
    unsigned long offset = (MAX_MEMORY - MIN_MEMORY) / num_frame * (index - start);
    return (void *)(MIN_MEMORY + offset);
}

Block *compute_block(void *addr_) {
    /*
    there are many possible block given an address
    so we need to check every possible block
    from the lowest level up to highest level, every level index have a ratio of 2n+1
    for example, if address is MIN_MEMORY, possible blocks are 0, 1, 3, 7...
    */
    unsigned long addr = (unsigned long) addr_;
    unsigned long offset = addr - MIN_MEMORY;
    unsigned long interval = MAX_MEMORY - MIN_MEMORY;    
    int level = 0;
    Block *res;

    while (1) {
        if (!(offset % interval)) break;
        else {
            level++;
            interval /= 2;
        }
    }
    int start_index = pow(2, level) - 1;
    offset /= interval;
    start_index += offset;
    res = (Block *)BLOCK_STRUCT_ADDR + start_index;

    while(1) {
        if (res->valid == -1) break;
        else {
            start_index = start_index * 2 + 1;
            res = (Block *)BLOCK_STRUCT_ADDR + start_index;
        }
    }
    return res;
}

void block_update(Block *block) {
    /*
    update when given block is allocated or merged or split
    */
    if (block->previous) {
        if (block->next) block->previous->next = block->next;
        else block->previous->next = NULL;
    }
    if (block->next) {
        if (block->previous) block->next->previous = block->previous;
        else block->next->previous = NULL;
    }

    block->previous = NULL;
    block->next = NULL;
}

int merge(Buddy *buddy, Block *deallocated) {
    if (!deallocated->index) return;

    char str[20]; // for itoa
    memset(str, 0, 20);

    int level = compute_level_with_index(deallocated->index);

    int buddy_is_even;

    // merge iteratively
    int valid;
    int parent_index;
    int offset;
    int merged = 0;
    Block *buddy_block;
    Block *child;
    Block *parent;
    Block *tmp;
    
    while (1) {
        // check if block buddy index is odd or even
        if (deallocated->index % 2) buddy_is_even = 1;
        else buddy_is_even = 0;

        if (buddy_is_even) {
            buddy_block = deallocated + 1;
            valid = buddy_block->valid;
            child = deallocated;
        }
        else {
            buddy_block = deallocated - 1;
            valid = buddy_block->valid;
            child = buddy_block;
        }

        if (valid == 1) {
            // compute parent Block address
            parent_index = (child->index - 1) / 2;
            offset = child->index - parent_index;
            parent = child - offset;

            // print log
            uart_puts("Block ");
            itoa(child->index, str);
            uart_puts(str);
            memset(str, 0, 20);
            uart_puts(" and Block ");
            itoa((child+1)->index, str);
            uart_puts(str);
            memset(str, 0, 20);
            uart_puts(" are merged into Block ");           
            itoa(parent->index, str);
            uart_puts(str);
            memset(str, 0, 20);
            uart_puts("\n");

            // merge
            deallocated->valid = 0;
            buddy_block->valid = 0;

            if (buddy[level].head && (buddy[level].head->index == deallocated->index)) {
                buddy[level].head = deallocated->next;
            }
            block_update(deallocated);

            if (buddy[level].head && (buddy[level].head->index == buddy_block->index)) {
                buddy[level].head = buddy_block->next;
            }      
            block_update(buddy_block);

            tmp = buddy[level-1].head;
            buddy[level-1].head = parent;
            parent->valid = 1;
            parent->next = tmp;

            if (tmp) tmp->previous = parent;

            deallocated = parent;
            level--;
            merged++;
        }
        else break;
    }
    if (!merged) return 0;
    else return 1;    
}

Block *release_redundant(Buddy *buddy, Block *unreleased, int level, int required_frame) {
    uart_puts("Releasing redundant block...\n");

    // iteratively split
    Block *left_child;
    Block *right_child;
    Block *head = NULL;
    Block *ret;
    Block *base = (Block *)BLOCK_STRUCT_ADDR;
    Block *tmp;

    int parent_index = unreleased->index;
    int child_level = level + 1;
    int child_frame;
    int right_frame = 0;
    int tmp_frame;
    int left_frame = required_frame;
    int flag;
    int tmp_valid;
    char str[20];
    memset(str, 0, 20);

    int max_release = WhichPowerOfTwo((MAX_MEMORY - MIN_MEMORY) / FRAME_SIZE);
    int release_array[max_release];

    while (1) {
        flag = 0;
        left_child = base + 2 * parent_index + 1;
        right_child = base + 2 * parent_index + 2;
        child_frame = (MAX_MEMORY - MIN_MEMORY) / FRAME_SIZE / pow(2, child_level);
        right_frame = left_frame - child_frame;

        // right child
        if (right_frame <= 0) {
            right_child->valid = 1;
            tmp = buddy[child_level].head;
            buddy[child_level].head = right_child;
            right_child->previous = NULL;
            right_child->next = tmp;
            if (tmp) tmp->previous = right_child;
            
            uart_puts("Block ");
            itoa(right_child->index, str);
            uart_puts(str);
            memset(str, 0, 20);
            uart_puts(" is released!\n");

            tmp_valid = left_child->valid;
            left_child->valid = 0;
            merge(buddy, right_child);
            left_child->valid = tmp_valid;
            tmp_frame = left_frame;
            flag++;
        }
        else {
            parent_index = right_child->index;
            tmp_frame = right_frame;
        }            
        
        // left child
        if (left_frame >= child_frame) {
            left_child->valid = -1;
            if (head) head->allocated_chain = left_child; 
            else {
                ret = left_child;                   
                head = left_child;
            }
            flag++;
            parent_index = right_child->index;
        }
        else parent_index = left_child->index;

        if (flag==2) break;

        left_frame = tmp_frame;
        child_level++;
    }
    return ret;
}

void free_frame(void *addr) {
    char str[20]; // for itoa
    memset(str, 0, 20);
    int merge_result = 0;
    int level;
    int total = 0;

    Buddy *buddy = (Buddy *)BUDDY_STRUCT_ADDR;
    Block *tmp;

    uart_puts("freed address: 0x");
    dec_hex((unsigned long)addr, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts(", Block id is: ");

    Block *deallocated = compute_block(addr);
    itoa(deallocated->index, str);
    uart_puts(str);
    memset(str, 0, 20);

    // print log
    Block *log = deallocated;
    while (1) {
        if (log->allocated_chain) {
            uart_puts(" and ");
            itoa(log->allocated_chain->index, str);
            uart_puts(str);
            memset(str, 0, 20);
            log = log->allocated_chain;
        }
        else break;
    }
    uart_puts("\n");

    while (1) {
        // insert to linked list
        deallocated->valid = 1;
        level = compute_level_with_index(deallocated->index);
        total += (MAX_MEMORY - MIN_MEMORY) / FRAME_SIZE / pow(2, level);
        
        tmp = buddy[level].head;
        buddy[level].head = deallocated;
        deallocated->next = tmp;
        if (tmp) tmp->previous = deallocated;
        
        merge_result += merge(buddy, deallocated);

        if (deallocated->allocated_chain) {
            tmp = deallocated;
            deallocated = deallocated->allocated_chain;
            tmp->allocated_chain = NULL;
        }
        else break;
    }
    uart_puts("Total freed frame: ");
    itoa(total, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts(", size: ");
    itoa(total*FRAME_SIZE, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts("B\n");

    if (!merge_result) uart_puts("No merge!!!\n");
}

void *allocate_frame(unsigned long n) {
    /*
    1. compute which level, if level n has available block?
    2. if true, delete and allocate a block, return the address
    3. if not, find level n-1, and so on
    4. split the level n-k block and insert to proper level, then goto 2
    5. release redundant frames
    */
    char str[20]; // for itoa
    memset(str, 0, 20);
    Buddy *buddy = (Buddy *)BUDDY_STRUCT_ADDR;
    unsigned long required_frame = frame_ceil(FRAME_SIZE, n);

    uart_puts("Requesting ");
    itoa(n, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts("B (");
    itoa(required_frame, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts(" frame)\n");

    // 1
    int level = compute_level(n);

    int available_level = 0;
    int current_split_level = 0;
    int out_of_mm = 0;
    Block *parent;
    Block *child1;
    Block *child2;
    int split = 1;

    // 3
    for (int i = level; i >= 0; i--) {
        if (buddy[i].head) {
            available_level = i;
            parent = buddy[i].head;
            out_of_mm = 1;
            break;
        }
    }
    if (!out_of_mm) uart_puts("Out of Memory!!!!!!!!!!!!!!\n");

    if (level == available_level) split = 0;

    // 4 iteratively split
    if (split) {
        int num_split_level = level - available_level;
    
        for (int i = 0; i < num_split_level; i++) {
            parent->valid = 0;
            buddy[available_level+i].head = parent->next;

            block_update(parent);

            current_split_level = available_level + i + 1;
            child1 = parent + parent->index + 1;
            child2 = child1 + 1;

            child1->valid = 1;
            child2->valid = 1;
            
            buddy[current_split_level].head = child1;
            child1->next = child2;
            child2->previous = child1;

            // print log            
            uart_puts("Block ");
            itoa(parent->index, str);
            uart_puts(str);
            memset(str, 0, 20);
            uart_puts(" is split into Block ");
            itoa(child1->index, str);
            uart_puts(str);
            memset(str, 0, 20);
            uart_puts(" and Block ");
            itoa(child2->index, str);
            uart_puts(str);
            memset(str, 0, 20);
            uart_puts("\n");           

            parent = child1;
        }        
    }

    // 2
    Block *allocated = buddy[level].head;

    // 5
    int total_frame = (MAX_MEMORY - MIN_MEMORY) / FRAME_SIZE;
    if (total_frame / pow(2, level) == required_frame) {
        allocated->valid = -1;
        buddy[level].head = allocated->next;
        block_update(allocated);
    }
    else {
        allocated->valid = 0;
        buddy[level].head = allocated->next;
        block_update(allocated);
        allocated = release_redundant(buddy, allocated, level, required_frame);
    }
    void *addr = compute_addr(level, allocated->index);
    
    // print log
    itoa(allocated->index, str);
    uart_puts("Block ");
    uart_puts(str);
    memset(str, 0, 20);

    while (allocated->allocated_chain) {
        uart_puts(", Block ");
        itoa(allocated->allocated_chain->index, str);
        uart_puts(str);
        memset(str, 0, 20);
        allocated = allocated->allocated_chain;
    }

    uart_puts(" is allocated. Total frame: ");
    itoa(required_frame, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts("\naddress: 0x");
    dec_hex((unsigned long)addr, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts("\n");
    return addr;
}

/*
--------------------------------
--------------------------------
--------Bucket Allocator-------
--------------------------------
--------------------------------
*/

typedef struct chunk_{
    /*
    chunk indices
             ----------------------------------------------------------------- 
             |                                                               |  1 frame 
             -----------------------------------------------------------------
    level 7  |               0               |               1               |  chunk size = 2KB
             -----------------------------------------------------------------
    level 6  |       0       |       1       |       2       |       3       |  chunk size = 1KB
             -----------------------------------------------------------------
    level 5  |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |  chunk size = 512B
             -----------------------------------------------------------------
    level 4  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10| 11| 12| 13| 14| 15|  chunk size = 256B
             -----------------------------------------------------------------
        .                                    .
        .                                    .
        .                                    .
    */
    struct chunk_ *next;
    int index;
} Chunk;

typedef struct bucket_ {
    unsigned long size;
    unsigned long available_chunk;
    unsigned long addr;
    struct bucket_ *next;
    struct bucket_ *previous;
    struct chunk_ *head;
} Bucket;

void bucket_init() {
    Bucket *buckets = (Bucket *)BUCKET_STRUCT_ADDR;
    Chunk *chunks = (Chunk *)CHUNK_STRUCT_ADDR;
    int num_chunk = 0;
    int chunk_index = 0;
    int num_bucket = WhichPowerOfTwo(FRAME_SIZE) - WhichPowerOfTwo(MIN_CHUNK_SIZE);
    unsigned long addr = (unsigned long)allocate_frame(num_bucket*FRAME_SIZE);

    for (int i = 0; i < num_bucket; i++) {
        num_chunk = FRAME_SIZE >> (WhichPowerOfTwo(MIN_CHUNK_SIZE) + i);
        buckets[i].size = pow(2, WhichPowerOfTwo(MIN_CHUNK_SIZE) + i);
        buckets[i].available_chunk = num_chunk;
        buckets[i].addr = addr + i * FRAME_SIZE;
        buckets[i].next = NULL;
        buckets[i].previous = NULL;        

        for (int j = 0; j < num_chunk; j++) {
            chunks[chunk_index+j].index = j;

            if (j != num_chunk-1) chunks[chunk_index+j].next = chunks + chunk_index+ j + 1;
            else chunks[chunk_index+j].next = NULL;            
        }
        buckets[i].head = chunks + chunk_index;
        chunk_index += num_chunk;
    }

    // print log
    unsigned long interval = (MAX_MEMORY - MIN_MEMORY) / FRAME_SIZE;
    int power = WhichPowerOfTwo(interval);
    unsigned long total_frame = pow(2, power);
    unsigned long total_index = pow(2, power+1) - 1;
    unsigned long start_index = total_index - total_frame;
    char str[20];
    memset(str, 0, 20);
    uart_puts("Initialize bucket chunk allocator complete!\n");
    itoa(num_bucket, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts(" frames are reserved for bucket allocator which are Block index ");
    itoa(start_index, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts(" to ");
    itoa(start_index+num_bucket-1, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts(".\n");
}


Bucket *allocate_bucket(int level) {
    /*
    compute total of bucket and new a bucket
    */
    int num_bucket = WhichPowerOfTwo(FRAME_SIZE) - WhichPowerOfTwo(MIN_CHUNK_SIZE);
    int real_num_bucket = 0;
    int num_chunk = 0;
    Bucket *bucket = (Bucket *)BUCKET_STRUCT_ADDR;
    Bucket *tmp;
    for (int i = 0; i < num_bucket; i++) {
        real_num_bucket++;
        num_chunk += FRAME_SIZE >> (WhichPowerOfTwo(MIN_CHUNK_SIZE) + i);

        tmp = bucket + i;
        while (tmp->next) {
            real_num_bucket++;
            num_chunk += FRAME_SIZE >> (WhichPowerOfTwo(MIN_CHUNK_SIZE) + i);
            tmp = tmp->next;
        }
    }
    void *addr = allocate_frame(FRAME_SIZE);

    Bucket *new_bucket = (Bucket *)BUCKET_STRUCT_ADDR + real_num_bucket;
    Chunk *new_chunk = (Chunk *)CHUNK_STRUCT_ADDR + num_chunk;

    Bucket *bucket_head = bucket + level;
    
    // insert to given level of bucket list
    if (!bucket_head->next) {
        bucket_head->next = new_bucket;
        new_bucket->previous = bucket_head;
        new_bucket->next = NULL;
    }
    else {
        Bucket *tmp = bucket_head->next;
        bucket_head->next = new_bucket;
        new_bucket->next = tmp;
        tmp->previous = new_bucket;
    }
    
    new_bucket->size = pow(2, WhichPowerOfTwo(MIN_CHUNK_SIZE) + level);
    new_bucket->available_chunk = FRAME_SIZE / new_bucket->size;
    new_bucket->head = new_chunk;
    new_bucket->addr = (unsigned long)addr;

    for (int i = 0; i < new_bucket->available_chunk; i++) {
        new_chunk[i].index = i;
        if (i != new_bucket->available_chunk-1) new_chunk[i].next = new_chunk + i + 1;
        else new_chunk[i].next = NULL;
    }
    char str[20];
    memset(str, 0, 20);
    uart_puts("\nBucket level ");
    itoa(level, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts(" is allocated\n");

    return new_bucket;
}

void *allocate_chunk(unsigned long n) {
    Bucket *bucket = (Bucket *)BUCKET_STRUCT_ADDR;
    int num_bucket = WhichPowerOfTwo(FRAME_SIZE) - WhichPowerOfTwo(MIN_CHUNK_SIZE);

    // find proper chunk size
    // bottom up
    int proper_level = 0;
    for (int i = 0; i < num_bucket; i++) {
        if (n < bucket[i].size) {
            proper_level = i;
            break;
        }
    }
    // check if there is available bucket given chunk size
    int available = 0;
    Bucket *owner = bucket + proper_level;
    while (1) {
        if (owner->available_chunk) {
            available = 1;
            break;
        }
        else {
            if (owner->next) owner = owner->next;
            else break;
        }
    }

    if (!available) {
        owner = allocate_bucket(proper_level);
    }

    if (!owner->available_chunk) uart_puts("allocate_chunk BUGGGGGG\n");

    Chunk *ret;
    if (owner->available_chunk == 1) {
        // keep the last chunk pointer instead of set to NULL
        owner->available_chunk--;
        if (!owner->head) uart_puts("last chunk doesn't point to null BUGGGGGG\n");
        ret = owner->head;
    }
    else {
        owner->available_chunk--;
        ret = owner->head;
        owner->head = ret->next;
    }
    unsigned long chunk_size = owner->size;
    void *addr = (void *)(owner->addr + ret->index * chunk_size);

    char str[20];
    memset(str, 0, 20);
    uart_puts("Requesting size ");
    itoa(n, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts("B\n");
    uart_puts("chunk size ");
    itoa(owner->size, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts("B is allocated\naddress is 0x");
    dec_hex((unsigned long)addr, str);
    uart_puts(str);
    memset(str, 0, 20);
    uart_puts("\n");

    return addr;
}

int free_chunk(void *addr) {
    /*
    brute force search bucket which has same prefix address
    */
    unsigned long prefix = (unsigned long)addr >> WhichPowerOfTwo(FRAME_SIZE);
    unsigned long bucket_prefix;
    int num_head_bucket = WhichPowerOfTwo(FRAME_SIZE) - WhichPowerOfTwo(MIN_CHUNK_SIZE);
    int if_chunk = 0;
    Bucket *bucket = (Bucket *)BUCKET_STRUCT_ADDR;
    Bucket *owner = bucket;
    Bucket *t;
    int flag = 1; // for breaking nested loop
    

    for (int i = 0; i < num_head_bucket && flag; i++) {
        owner = bucket + i;
        bucket_prefix = owner->addr >> WhichPowerOfTwo(FRAME_SIZE);
        if (prefix == bucket_prefix) {
            if_chunk = 1;
            break;
        }
        t = owner;
        while (t->next) {
            t = t->next;
            bucket_prefix = t->addr >> WhichPowerOfTwo(FRAME_SIZE);
            if (prefix == bucket_prefix) {
                if_chunk = 1;
                flag = 0;
                owner = t;
                break;
            }
        }
    }
    if (!if_chunk) {
        return 0;
    }
    else {
        // compute offset
        unsigned long offset = (unsigned long)addr - owner->addr;
        int index = offset / owner->size;

        Chunk *head = owner->head;
        int current_index = head->index;

        Chunk *released = head + (index - current_index);
        
        unsigned long max_chunk = FRAME_SIZE / owner->size;
        if (!owner->available_chunk) owner->head = released;
        else {
            Chunk *tmp = owner->head;
            owner->head = released;
            released->next = tmp;
        }
        owner->available_chunk++;
        
        // print log
        char str[20];
        memset(str, 0, 20);
        uart_puts("freed address: 0x");
        dec_hex((unsigned long)addr, str);
        uart_puts(str);
        memset(str, 0, 20);
        uart_puts(", chunk size ");
        itoa(owner->size, str);
        uart_puts(str);
        memset(str, 0, 20);
        uart_puts("B\n");

        if (owner->available_chunk == max_chunk) free_bucket(owner);        

        return 1;
    }
}

void free_bucket(Bucket *freed) {
    /*
    check if freed bucket is reserved
    if true, hold the bucket
    */
    if (freed->previous) {
        
        // print log
        char str[20];
        memset(str, 0, 20);
        int level = WhichPowerOfTwo(freed->size / MIN_CHUNK_SIZE);
        uart_puts("Bucket level ");
        itoa(level, str);
        uart_puts(str);
        memset(str, 0, 20);
        uart_puts(" is freed, ");

        free_frame(freed->addr);    
    }
}

/*
--------------------------------
--------------------------------
--------Dynamic Allocator-------
--------------------------------
--------------------------------
*/

void allocator_init() {
    frame_init();
    bucket_init();
}

void *malloc(unsigned long n) {
    Bucket *bucket = (Bucket *)BUCKET_STRUCT_ADDR;
    int num_bucket = WhichPowerOfTwo(FRAME_SIZE) - WhichPowerOfTwo(MIN_CHUNK_SIZE);
    unsigned long max_bucket_size = (bucket + num_bucket - 1)->size;
    void *addr;
    uart_puts("\n--------malloc---------\n");

    if (max_bucket_size != FRAME_SIZE/2) uart_puts("size BUGGGGG!!");

    if (n > max_bucket_size) addr = allocate_frame(n);
    else addr = allocate_chunk(n);
    uart_puts("------malloc end-------\n");
    return addr;
}

void free(void *addr) {
    uart_puts("\n--------free---------\n");
    int res = free_chunk(addr);
    if (!res) free_frame(addr);
    uart_puts("------free end-------\n");
}