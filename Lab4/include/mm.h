#ifndef __MM_H_
#define __MM_H_
#include "mm.h"
#include "uart.h"
#include "stdint.h"
#include "stddef.h"



void *kmalloc(uint64_t size);
void kfree(void *ptr);

#endif