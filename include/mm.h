#ifndef _MM_H_
#define  _MM_H_

void mm_init();
void* kmalloc(unsigned int);
void kfree(void*);
void buddy_print();

#endif
