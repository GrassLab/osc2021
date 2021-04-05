#pragma once

#define SLAB_NUM_SLAB_SIZES 6

#define BUDDY_MAX_EXPONENT 18 // 1GB
// #define BUDDY_MAX_EXPONENT 10
// #define BUDDY_MAX_EXPONENT 5

#define BUDDY_NUM_FREE_LISTS (BUDDY_MAX_EXPONENT + 1)

// #define MEMORY_START 0x90000
#define MEMORY_START 0x0

// SlabAllocator
//    manage slabs with the same allocation size,
//    A slab is a frame allocated with small objects with same size
//      + @SLAB_MAX_SLOTS: The number of pages available for a single slab
//        -> Frame is 4kb by architecture.
//          CortexA53 is 16 bytes aligned, so the here 16 bytes is set to be
//          the minimum size available for objects in slab.
//        -> Therefore, the maximum slots in slab is 4096/16 = 256
#define SLAB_MAX_SLOTS 256

//  The size of slab range from 16(2^4) to 512(2^9) bytes
#define SLAB_OBJ_MIN_SIZE_EXP 4
#define SLAB_OBJ_MAX_SIZE_EXP 9