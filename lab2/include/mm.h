struct page {
    unsigned long addr;
    struct page *next;
    // buddy might be intermediate of a free-link-list,
    // when we are going to coalesce buddy, then buddy
    // should leave the list. Hence we need *prev to
    // know how to deal the case of intermediate buddy.
    struct page *prev;
    int index; // index in page_frames
    int free; // 1: True, 0: False
    int blk_odr; 
    // In case of allocated, if page is head,
    // then blk_odr == x means compound block
    // size is 2^x. if page is -1, means page
    // is tail of compound block.
    // e.g. 3 -> 2^3;
    //     -1 -> compound tail page;
    //     -2 -> free_list head;
};

struct page_descriptor {
    struct page_descriptor *next, *prev;
    struct page *frame;
    char *bitmap_start;
    int bitmap_len; // bitmap_len is length of bitmap (count in byte).
    char *chunk_start;
    int chunk_size;
    int chunk_num;
};

struct region_descriptor {
	struct region_descriptor *next, *prev;
	unsigned long addr;
	unsigned long size;
	int free; // 1 for free, 0 for not
	int used; // 1 for True, 0 for not
};

void buddy_system_init(unsigned long mem_base);
struct page *get_free_frames(int nr);
int free_frames(struct page *frame);
int remove_from_list(struct page *frame);
int insert_into_list(struct page *frame);
int show_m();
int show_mm();
int show_sm();

int set_pd(struct page_descriptor *pd,
	struct page *frame, int chunk_size, int chunk_num);
int pd_list_grow(struct page_descriptor *pd, int chunk_size, int chunk_num);
int remove_from_pdlist(struct page_descriptor *pd);
int insert_into_pdlist(struct page_descriptor *pd_new,
	struct page_descriptor *pd_prev);
int get_free_index(struct page_descriptor *pd_walk);
char *kmalloc(int size);
int kfree(char* ptr);
void dynamic_mem_init();
int rd_init();

