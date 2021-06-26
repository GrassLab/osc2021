#ifndef _MM_H_
#define  _MM_H_

#define PAGE_SIZE   4096

void mm_init();
void* kmalloc(unsigned int);
void kfree(void*);
void buddy_print();

#endif
