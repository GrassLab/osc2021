#ifndef DYNAMIC_H
#define DYNAMIC_H
#include <types.h>

#define DYNAMIC_BIN_MAX 0x40
#define DYNAMIC_BIN_MIN_SLOT 0x10 

#define DYNAMIC_CHUNK_HEADER_OFFSET 0x20
/**
 * 0x20, 0x30, 0x40 ... 0x400
 */

struct dynamic_struct
{
  //top chunk for unallocated area
  struct dynamic_chunk *top_chunk;
  //free bin linked list
  struct dynamic_chunk *bins[DYNAMIC_BIN_MAX]; 
  //store chunk size larger than maximum bin size
  struct dynamic_chunk *unsorted_bin;
} dynamic_system;

struct dynamic_chunk 
{
  size_t prev_size; //previous chunk size
  size_t size; //low 1 bit for inuse bit
  struct dynamic_chunk *next;
};

void dynamic_init();
void* dynamic_malloc(size_t size);
void* dynamic_find_free_chunk(int idx);
void dynamic_free(void* address);
void* dynamic_top_chunk_malloc(int idx);
void dynamic_status();
void dynamic_top_chunk_free();
int dynamic_remove_chunk(void* address, size_t size);
int dynamic_request_new_page();
void* dynamic_unsorted_bin_malloc(int idx);
#endif