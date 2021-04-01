#include "buddy.h"
#include "uart.h"

uint64_t base_addr = 0x00ff0000000000;
uint64_t pool_ptr = 0;
uint64_t frame_size = 1 << 12; // 4k
struct buddy_node_t buddy_node_pool[1024];
struct buddy_node_t* buddy_entry;
struct status_node_t* free_node_entry[MAX_BSIZE + 1];

void buddy_init()
{
    print_string("Buddy system init....\n");
    buddy_entry = new_buddy(0, 1 << MAX_BSIZE);
    for (int i = 0; i < MAX_BSIZE; i++) {
        free_node_entry[i] = 0;
    }
    free_node_entry[MAX_BSIZE] = &buddy_entry->status;
    print_string("Starting address: ");
    print_address(free_node_entry[MAX_BSIZE]->buddy_node->addr);
    print_string("\n");
}

struct buddy_node_t* new_buddy(uint64_t addr, uint64_t size)
{
    struct buddy_node_t* node = &buddy_node_pool[pool_ptr];
    node->index = pool_ptr;
    pool_ptr++;
    node->addr = addr + base_addr;
    node->size = size * frame_size;
    node->free = true;
    node->split = false;
    node->status.val = __builtin_ctz(size); // log(2)
    node->status.buddy_node = node;
    if (size == 1) {
        node->left = 0;
        node->right = 0;
        return node;
    }
    node->left = new_buddy(addr, size / 2);
    node->right = new_buddy(addr + size / 2 * frame_size, size / 2);
    node->left->parent = node;
    node->right->parent = node;
    return node;
}

void free_node_push(struct status_node_t* node)
{
    node->next = free_node_entry[node->val];
    free_node_entry[node->val]->prev = node;
    free_node_entry[node->val] = node;
}

struct status_node_t* free_node_pop(uint64_t size)
{
    struct status_node_t* node = free_node_entry[size];
    if (node->next) {
        node->next->prev = 0;
    }
    free_node_entry[size] = node->next;
    return node;
}

void free_node_delete(struct status_node_t* node)
{
    if (!node->prev && !node->next) {
        free_node_entry[node->val] = 0;
        return;
    }
    if (node->prev) {
        node->prev->next = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }
}

struct buddy_node_t* buddy_system(uint64_t size)
{
    print_string("Current size of block: ");
    print_integer((1 << size) * 4);
    print_string("KB\n");
    if (size > MAX_BSIZE) {
        print_string("Cannot allocate\n");
        return 0;
    }
    if (!free_node_entry[size]) {
        print_string("Current size of block is empty, Seachering for bigger one\n");
        struct buddy_node_t* node = buddy_system(size + 1);
        node->free = false;
        node->left->free = false;
        free_node_push(&node->right->status);
        return node->left;
    } else {
        struct status_node_t* status_node = free_node_pop(size);
        status_node->buddy_node->free = false;
        print_string("Allocation successful\n");
        print_string("Address: ");
        print_address(status_node->buddy_node->addr);
        print_string("\n");
        return status_node->buddy_node;
    }
}

struct buddy_node_t* buddy_alloc(uint64_t size)
{
    print_string("Allocation start\n");
    struct buddy_node_t* node = buddy_system(size);
    return node;
}

void buddy_free(struct buddy_node_t* node)
{
    print_string("Start to free buddy block\n");
    print_string("Current size of block: ");
    print_integer(node->size >> 10);
    print_string("KB\n");
    node->free = true;
    if (node->parent->left == node && node->parent->right->free) {
        print_string("Contiguous free block found, start merging\n");
        node->parent->free = true;
        free_node_delete(&node->status);
        buddy_free(node->parent);
    } else if (node->parent->right == node && node->parent->left->free) {
        print_string("Contiguous free block found, start merging\n");
        node->parent->free = true;
        free_node_delete(&node->status);
        buddy_free(node->parent);
    } else {
        free_node_push(&node->status);
        print_string("Successfully free\n");
    }
}
