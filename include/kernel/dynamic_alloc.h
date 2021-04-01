#ifndef _DYNAMIC_ALLOC_H
#define _DYNAMIC_ALLOC_H
struct chunk_256 {
      struct chunk_256 *next;
      char data[248]; // 256 - 8
};
struct chunk_128 {
      struct chunk_128 *next;
      char data[120]; // 128 - 8
};
struct chunk_64 {
      struct chunk_64 *next;
      char data[56]; // 64 - 8
};
struct chunk_32 {
      struct chunk_32 *next;
      char data[24]; // 32 - 8
};
struct chunk_16 {
      struct chunk_16 *next;
      char data[8]; // 16 - 8
};

struct dym_frame {
      struct chunk_256 chunk_256[3];
      struct chunk_128 chunk_128[6];
      struct chunk_64 chunk_64[12];
      struct chunk_32 chunk_32[24];
      struct chunk_16 chunk_16[64];
      // total 4096
};

struct pool {
      struct chunk_256 *chunk_256;
      struct chunk_128 *chunk_128;
      struct chunk_64 *chunk_64;
      struct chunk_32 *chunk_32;
      struct chunk_16 *chunk_16;
};


void *malloc(int size);
void *get_chunk(int chunk_size);
void augment_pool();
void pool_stat();
void free(void *addr);
void append_to_pool(int size, void* record);
#endif