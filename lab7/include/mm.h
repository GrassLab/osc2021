#ifndef __MM_H_
#define __MM_H_

void init_buddy();
void *kmalloc(unsigned int size);
void *kcalloc(unsigned int size);
void kfree(void *ptr);
size_t get_alloc_size(void *ptr);

#endif