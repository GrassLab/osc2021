#include <kernel/fat32.h>
#include <kernel/sdhost.h>
#include <kernel/memory_func.h>
#include <kernel/mini_uart.h>
#include <kernel/string.h>

void get_mbr(struct mbr *mbr, char *buffer) {
    readblock(0, buffer);
    char buffer2[20];
    for (int i = 0 ;i < 4; i++) {
        memcpy(&mbr->entry[i].first_sector_addr, &buffer[446 + i * 16 + 8], 4);
        memcpy(&mbr->entry[i].total_sectors, &buffer[446 + i * 16 + 12], 4);
        io() << i << " first_sector_addr: " << u64tohex(mbr->entry[i].first_sector_addr, buffer2, sizeof(buffer2)) << "\r\n";
        io() << i << " total_sectors: " << u64tohex(mbr->entry[i].total_sectors, buffer2, sizeof(buffer2)) << "\r\n";
    }
}

void get_fat(struct fat32 *fat, char *sector_buffer) {
    struct mbr mbr;
    char buffer2[20];
    io() << "Checkpoint 1\r\n";
    get_mbr(&mbr, sector_buffer);
    io() << "Checkpoint 2\r\n";
    int fat32_start_sector = mbr.entry[0].first_sector_addr;
    io() << "Checkpoint 3\r\n";
    readblock(fat32_start_sector, sector_buffer);
    io() << "Checkpoint 4\r\n";
    memcpy(&fat->bytes_pre_logical_sector, &sector_buffer[0xb], 2);
    memcpy(&fat->logical_sector_per_cluster, &sector_buffer[0xd], 1);
    memcpy(&fat->reserved_logical_sector_size, &sector_buffer[0xe], 2);
    memcpy(&fat->fat_count, &sector_buffer[0x10], 1);
    memcpy(&fat->fat_size, &sector_buffer[0x24], 4);
    memcpy(&fat->root_dir_start, &sector_buffer[0x2c], 4);
    memcpy(&fat->fs_info_size, &sector_buffer[0x30], 2);

    fat->fat_start_block = fat32_start_sector + fat->reserved_logical_sector_size;
    fat->data_start_block = fat->fat_start_block + fat->fat_count * fat->fat_size;
    io() << "bytes_pre_logical_sector: " << u64tohex(fat->bytes_pre_logical_sector, buffer2, sizeof(buffer2)) << "\r\n";
    io() << "logical_sector_per_cluster: " << u64tohex(fat->logical_sector_per_cluster, buffer2, sizeof(buffer2)) << "\r\n";
    io() << "reserved_logical_sector_size: " << u64tohex(fat->reserved_logical_sector_size, buffer2, sizeof(buffer2)) << "\r\n";
    io() << "fat_count: " << u64tohex(fat->fat_count, buffer2, sizeof(buffer2)) << "\r\n";
    io() << "fat_size: " << u64tohex(fat->fat_size, buffer2, sizeof(buffer2)) << "\r\n";
    io() << "root_dir_start: " << u64tohex(fat->root_dir_start, buffer2, sizeof(buffer2)) << "\r\n";
    io() << "fs_info_size: " << u64tohex(fat->fs_info_size, buffer2, sizeof(buffer2)) << "\r\n";
    io() << "fat_start_block: " << u64tohex(fat->fat_start_block, buffer2, sizeof(buffer2)) << "\r\n";
    io() << "data_start_block: " << u64tohex(fat->data_start_block, buffer2, sizeof(buffer2)) << "\r\n";
    io() << "Checkpoint 5\r\n";
    readblock(fat->fat_start_block, sector_buffer);
    io() << "Checkpoint 6\r\n";
    readblock(fat->data_start_block, sector_buffer);
    io() << "Checkpoint 7\r\n";
}
