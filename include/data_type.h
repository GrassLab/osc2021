#ifndef DATA_TYPE_H
#define DATA_TYPE_H

typedef unsigned long u64;
typedef unsigned int u32;
#define NULL 0
#define page_size 0x1000
#define page_mask 0xfffffffffffff000
#define aligned16(size) (((size + 0xf) & 0xffffffffffffff0))

#endif
