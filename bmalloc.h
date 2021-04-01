#ifndef BMALLOC_H
#define BMALLOC_H

#include <stdint.h>

void* bmalloc(uint64_t size);
void bfree(void* addr);

#endif