/* max allocation size is 2 GB */
#define MAX_ALLOC_LOG2 31
#define MAX_ALLOC_SIZE (1 << MAX_ALLOC_LOG2)
/* each page is 4KB */
#define PAGE_SIZE_LOG2 12
#define PAGE_NUM (1  << (MAX_ALLOC_LOG2 - PAGE_SIZE_LOG2))

#define BUCKETS_NUM MAX_ALLOC_LOG2 - PAGE_SIZE_LOG2 + 1
#define NULL ((void*)0)


/* linked list for blocks */
typedef struct node {
    int index;
    struct node *next;
    struct node *prev;
} Node;

/* buckets store linked-lists for blocks with different size */
static Node buckets[BUCKETS_NUM];

/*
 * Initialize a list to empty. Because these are circular lists, an "empty"
 * list is an entry where both links point to itself.
 */
static void node_init(Node *node, int index) {
    node->index = index;
    node->next = node;
    node->prev = node;
}

/* append the provided entry to the end of list_t */
static void list_push(Node *list, Node *node) {
    Node *prev = list->prev;
    node->prev = prev;
    node->next = list;
    prev->next = node;
    list->prev = node;
}

/* remove the provided entry from list_t */
static void list_remove(Node *node) {
    Node *prev = node->prev;
    Node *next = node->next;
    prev->next = next;
    next->prev = prev;
}

static void init_list_head(Node *list) {
	list->next = list;
	list->prev = list;
}

static void buckets_init() {
    for(int i = 0; i < BUCKETS_NUM; i++) {
      init_list_head(&buckets[i]);
    }
    Node *head = NULL;
    node_init(head, 0);
    list_push(&buckets[0], head);
}

static int split_block(Node *node, int order, int block_size) {
	int hi = node->prev->index + (1 << block_size); 
	int lo = node->prev->index;
	while(block_size > order) {
		int mid = lo + (hi-lo)/2;
		block_size = block_size - 1;

		/* add the bottom half of the block to free_list */
		Node *new_node = NULL;
		node_init(new_node, mid);
		list_push(&buckets[BUCKETS_NUM - block_size -1], new_node);

		/* recursive */
		hi = mid;
	}
	return block_size;
}

/* find a block from buckets which has the size at least 2^order * 4KB */
static int get_page_from_buckets(int order) {
    for(int i = BUCKETS_NUM - order - 1; i >= 0 ; i--) {
		Node *list_head = &buckets[i];
		if(list_head == list_head->prev) continue; // buckets[i] is empty
		else {
      		int block_index = list_head->prev->index;
			if(BUCKETS_NUM-i-1 > order) {
				int new_block_size = split_block(list_head->prev, order, BUCKETS_NUM-i-1);
			}
			list_remove(list_head->prev);

        	return block_index;
      	}
    }
    return -1; // No block can use
}