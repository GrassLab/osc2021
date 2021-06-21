#include <kernel/fat32.h>
#include <kernel/sdhost.h>
#include <kernel/memory_func.h>

void get_mbr(struct mbr *mbr, char *buffer) {
    readblock(0, buffer);
    for (int i = 0 ;i < 4; i++) {
        memcpy(&mbr->entry[i].first_sector_addr, &buffer[446 + i * 16 + 8], 4);
        memcpy(&mbr->entry[i].total_sectors, &buffer[446 + i * 16 + 12], 4);
    }
}

void get_fat(struct fat32 *fat, char *sector_buffer) {
    struct mbr mbr;
    get_mbr(&mbr, sector_buffer);
    int fat32_start_sector = mbr.entry[0].first_sector_addr;
    readblock(fat32_start_sector, sector_buffer);
    memcpy(&fat->bytes_pre_logical_sector, &sector_buffer[0xb], 2);
    memcpy(&fat->logical_sector_per_cluster, &sector_buffer[0xd], 1);
    memcpy(&fat->reserved_logical_sector_size, &sector_buffer[0xe], 2);
    memcpy(&fat->fat_count, &sector_buffer[0x10], 1);
    memcpy(&fat->fat_size, &sector_buffer[0x24], 4);
    memcpy(&fat->root_dir_start, &sector_buffer[0x2c], 4);
    memcpy(&fat->fs_info_size, &sector_buffer[0x30], 2);

    fat->fat_start_block = fat32_start_sector + fat->reserved_logical_sector_size;
    fat->data_start_block = fat->fat_start_block + fat->fat_count * fat->fat_size;
    readblock(fat->fat_start_block, sector_buffer);
    readblock(fat->data_start_block, sector_buffer);
}
