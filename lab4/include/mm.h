#ifndef __MM_H_
#define __MM_H_

void init_buddy();
void *kmalloc(unsigned int size);
void kfree(void *ptr);

#endif