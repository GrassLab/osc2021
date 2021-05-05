#ifndef _KMALLOC_H_
#define _KMALLOC_H_
#include <types.h>
#include <dynamic.h>
#include <buddy.h>

void* varied_malloc(size_t size);
void varied_free(void* address);

#endif