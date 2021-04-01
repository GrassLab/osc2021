
typedef struct page
{
	unsigned long addr;
	struct page *next;
}Page;
void alloc_page_init(unsigned long addr_low, unsigned long addr_high);
unsigned int alloc_page(int size);
void free_page(int addr, int size);