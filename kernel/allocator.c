#include "uart.h"
#include "utils.h"
#include "allocator.h"
#include "base.h"

/* buckets store linked-lists for blocks with different size */
static Node buckets[BUCKETS_NUM];
extern long __stack_top;

static void node_init(Node *node, int index) {
    node->index = index;
	node->next = NULL;
}

/* append the provided entry to buckets[i] */
static void list_push(Node *list, Node *node) {
	Node *p = list;
	while(p->next) {
		p = p->next;
	}
    p->next = node;
}

/* 
* remove the provided node in buckets[i]
* to use this function, make sure the node with the index exist in list
*/
static void list_remove(Node *list, Node *node) {
	Node *p = list;
    while(p->next != node) {
		p = p->next;
	}

	p->next = node->next;
	node->next = NULL;
}

/* 
* remove the node with provided index 
* to use this function, make sure the node with the index exist in list
*/
static void list_remove_index(Node *list, int index) {
	Node *p = list;
	while(p->next->index != index && p != NULL) {
		p = p->next;
	}

	p->next = p->next->next;
	p->next->next = NULL;
}

void print_free_list() {
	for(int i = 0; i < 8; i++) {
		if(i==0) uart_puts("---16 bytes---\n");
		uart_puts("pool ");
		uart_puts_int(i);
		uart_puts(": ");
		Node *head = &buckets[i];
		Node *p = head->next;
		while(p != NULL) {
			uart_puts_int(p->index);
			uart_puts(" -> ");
			p = p->next;
		}
		uart_puts("\n");
	}
	
	for(int i = 8; i < BUCKETS_NUM; i++) {
		if(i==8) uart_puts("---4KB---\n");
		uart_puts("list ");
		uart_puts_int(i-8);
		uart_puts(": ");
		Node *head = &buckets[i];
		Node *p = head->next;
		while(p != NULL) {
			uart_puts_int((p->index) >> 8);
			uart_puts(" -> ");
			p = p->next;
		}
		uart_puts("\n");
	}
	uart_puts("\n");
}

void init_buckets() {
	for(int i = 0; i < BUCKETS_NUM; i++) {
		Node *list_head = &buckets[i];
		list_head->next = NULL;
		list_head->index = -1;
	}
	Node *p = (Node *)((unsigned long)BASE_MEM + 4096*0);
    node_init(p, 0);
    list_push(&buckets[BUCKETS_NUM-1], p);
	uart_puts("[System info]: init frame freelist...\n");
	// print_free_list();
}

void split_block(Node *p, int order, int block_size) {
	//uart_puts("=====Release redundant block=====\n");
	int hi = p->index + (1 << block_size); 
	int lo = p->index;
	while(block_size > order) {
		int mid = lo + (hi-lo)/2;
		block_size = block_size - 1;

		/* add the bottom half of the block to free_list */
		Node *new_node = (Node *)((unsigned long)BASE_MEM + (1 << PAGE_SIZE_LOG2)*mid);
		node_init(new_node, mid);
		list_push(&buckets[block_size], new_node);
		
		/* recursive */
		hi = mid;
		/*
		Node *list_head = &buckets[block_size];
		if(block_size >> 3) {
			uart_puts("[list ");
			uart_puts_int(block_size>>3);
			uart_puts("] add block: index ");
			uart_puts_int((list_head->next->index) >> 8);
			uart_puts(", address 0x");
			uart_puts_hex((unsigned long)new_node);
			uart_puts("\n");
		}
		
		else {
			uart_puts("[pool ");
			uart_puts_int(block_size);
			uart_puts("] add block: index ");
			uart_puts_int(list_head->next->index);
			uart_puts(", address 0x");
			uart_puts_hex((unsigned long)new_node);
			uart_puts("\n");
		}
		*/
		// print_free_list();
	}
	//uart_puts("\n");
}

/* find a block from buckets which has the size at least 2^order * 4KB */
Node* get_page_from_buckets(Node *p, int bucket_pagen, int required_pagen) {
	// uart_puts("=====Finding free block=====\n");
    for(int i = bucket_pagen; i < BUCKETS_NUM ; i++) {
		Node *list_head = &buckets[i];
		Node *p = list_head->next;
		/* If bucket[i] is not empty */
		if(p) {
			/*
      		int block_index = p->index;
			if(i >> 3) {
				uart_puts("[list ");
				uart_puts_int(i-8);
				uart_puts("] find block: index ");
				uart_puts_int(block_index >> 8);
				uart_puts(", address 0x");
				uart_puts_hex((unsigned long)p);
				uart_puts("\n");
				uart_puts("\n");
			}

			else {
				uart_puts("[pool ");
				uart_puts_int(i);
				uart_puts("] find block: index ");
				uart_puts_int(block_index);
				uart_puts(", address 0x");
				uart_puts_hex((unsigned long)p);
				uart_puts("\n");
				uart_puts("\n");
			}
			*/
			list_remove(list_head, p);
			if(i > required_pagen) split_block(p, required_pagen, i);
        	return p;
      	}
    }
    return NULL; // No block can use
}


void* kmalloc(unsigned int size) {
	int page_n = (size >> PAGE_SIZE_LOG2);
    page_n += size % (1 << PAGE_SIZE_LOG2) == 0 ? 0 : 1;
	int kbpage_n = (size >> 12);
    kbpage_n += size % (1 << 12) == 0 ? 0 : 1;
	/*
    uart_puts("=====KMalloc=====\n");
    uart_puts("need ");
    uart_puts_int(size);
    uart_puts(" bytes, equal to ");
    uart_puts_int(page_n);
    uart_puts(" 16-bytes or ");
	uart_puts_int(kbpage_n);
	uart_puts(" 4KB pages\n");
	uart_puts("\n");
	*/
	// print_free_list();

	unsigned int start_bucket = 0;
	int temp = page_n;
	while(temp >> 1) {
		start_bucket += 1;
		temp = temp >> 1;
	}
	start_bucket += page_n & (page_n-1) ? 1 : 0;

    for(int i = start_bucket; i < BUCKETS_NUM; i++) {
        Node *list_head = &buckets[i];
        /* buckets is not empty */
        if(list_head->next) {
            Node *p = get_page_from_buckets(list_head, i, start_bucket);
			/*
			uart_puts("=====KMalloc success=====\n");
            uart_puts("allocate at 0x");
            uart_puts_hex((unsigned long)p);
			uart_puts(", index ");
			uart_puts_int(p->index);
            uart_puts("\n");
			uart_puts("\n");
			*/
            return p;
        }
    }
	// print_free_list();
    uart_puts("(error) kmalloc fail!\n");
	while(1);
    return 0;
}

int merge_block(int index, int bucket_index) {
	int merge = 0;
	for(int i = bucket_index; i < BUCKETS_NUM-1; i++) {
		int go_to_next_bucket = 0;
		int target = index ^ (1<<i);
		Node *curr_bucket = &buckets[i];
		Node *p = curr_bucket->next;

		/* search the bucket until reach the end */
		while(p) {
			if(p->index == target) {
				go_to_next_bucket = 1;
				merge = 1;
				list_remove(curr_bucket, p);
				/* from second times merge, we have to remove two node in the same buckets */
				if(i>bucket_index) list_remove_index(curr_bucket, index);

				/* merge the block index with block target, then add the new merged block into next level bucket */
				index = target > index ? index : target;
				Node *next_buckets = &buckets[i+1];
				Node *new_node = (Node *)((unsigned long)BASE_MEM + index * PAGE_SIZE);
				node_init(new_node, index);
				list_push(next_buckets, new_node);

				/*
				if(i >> 3) {
					uart_puts("[list ");
					uart_puts_int(i-8);
					uart_puts("] find buddy: index ");
					uart_puts_int(target >> 8);
					uart_puts("\n");
					uart_puts("[list ");
					uart_puts_int(i+1-8);
					uart_puts("] add block: index ");
					uart_puts_int(index >> 8);
					uart_puts("\n");
				}
				else {
					uart_puts("[pool ");
					uart_puts_int(i);
					uart_puts("] find buddy: index ");
					uart_puts_int(target);
					uart_puts("\n");
					uart_puts("[pool ");
					uart_puts_int(i+1);
					uart_puts("] add block: index ");
					uart_puts_int(index);
					uart_puts("\n");
				}
				*/
				// print_free_list();
			}
			p = p->next;
		}
		/* no merge happend, no need to search next bucket */
		if(!go_to_next_bucket) break;
	}
	if(merge) return index;
	return -1;
}

void free_page(unsigned long addr, unsigned int size) {
	// 用addr反推index
	int index = (addr-BASE_MEM) >> PAGE_SIZE_LOG2;
	int page_n = (size >> PAGE_SIZE_LOG2);
    page_n += size % (1 << PAGE_SIZE_LOG2) == 0 ? 0 : 1;
	unsigned int bucket_index = 0;
	int temp = page_n;
	while(temp >> 1) {
		bucket_index += 1;
		temp = temp >> 1;
	}
	bucket_index += page_n & (page_n-1) ? 1 : 0;
	/*
	if(bucket_index >> 3) {
		uart_puts("=====Free block=====\n");
		uart_puts("free the block start from index ");
		uart_puts_int(index >> 8);
		uart_puts(", list ");
		uart_puts_int(bucket_index-8);
	}
	else {
		uart_puts("=====Free block=====\n");
		uart_puts("free the block start from index ");
		uart_puts_int(index);
		uart_puts(", pool ");
		uart_puts_int(bucket_index);
	}
	uart_puts("\n");
	*/
	int merge = -1;
	merge = merge_block(index, bucket_index);
	/* no merge */
	if(merge == -1) {
		Node *list_head = &buckets[bucket_index];
		Node *new_node = (Node *)addr;
		node_init(new_node, index);
		list_push(list_head, new_node);
		/*
		uart_puts("buddy is not free\n");

		if(bucket_index >> 3) {
			uart_puts("[list ");
			uart_puts_int(bucket_index-8);
			uart_puts("] add block: index ");
			uart_puts_int(index >> 8);
			uart_puts(", address 0x");
			uart_puts_hex((unsigned int)addr);
		}
		
		else {
			uart_puts("[pool ");
			uart_puts_int(bucket_index);
			uart_puts("] add block: index ");
			uart_puts_int(index);
			uart_puts(", address 0x");
			uart_puts_hex((unsigned int)addr);
		}
		uart_puts("\n");
		uart_puts("\n");
		*/
	}
}