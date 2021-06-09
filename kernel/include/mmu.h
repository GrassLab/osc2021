#pragma once

#include <stdint.h>

#define VA2PA(addr) ((uint64_t)(addr) & (uint64_t)0x0000ffffffffffff)
#define PA2VA(addr) ((uint64_t)(addr) | (uint64_t)0xffff000000000000)

void init_page_table(uint64_t **table);
void update_page_table(uint64_t *pgd, uint64_t virtual_addr,
                       uint64_t physical_addr, int permission);
