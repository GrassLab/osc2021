#pragma once

#include <types.h>

struct mbr_table_entry
{
    uint32_t first_sector_addr;
    uint32_t total_sectors;
};

struct mbr
{
    struct mbr_table_entry entry[4];
};

struct fat32 {
    uint16_t bytes_pre_logical_sector;
    uint8_t logical_sector_per_cluster;
    uint16_t reserved_logical_sector_size;
    uint8_t fat_count;
    uint32_t fat_size; // 0x24
    uint32_t root_dir_start; // 0x2c
    uint16_t fs_info_size; // 0x30
    
    uint32_t fat_start_block;
    uint32_t data_start_block;
};

void get_fat(struct fat32 *fat, char *sector_buffer);
