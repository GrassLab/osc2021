#ifndef FAT32_H
#define FAT32_H
#include "vfs.h"
typedef struct fat32_partition
{
    char status_flag;
    char partition_begin_head;
    unsigned short partition_begin_sector;
    char type;
    char partition_end_head;
    unsigned short partition_end_sector;
    unsigned int starting_sector;
    unsigned int number_of_sector;

    unsigned int root_sector_abs;

} fat32_partition_t;

typedef struct boot_sector
{
    char jump[3];                   // 0-2
    char oem[8];                    // 3-10
    char bytes_per_logic_sector_0;  // 11
    char bytes_per_logic_sector_1;  // 12
    char logic_sector_per_cluster;  // 13
    unsigned short n_reserved_sectors;       // 14-15
    char n_file_alloc_tabs;         // 16
    char n_root_dir_entries_1;      // 17
    char n_root_dir_entries_2;      // 18
    unsigned short n_logical_sectors_16;     // 19-20
    char media;                     // 21
    unsigned short n_sector_per_fat_16;      // 22-23
    unsigned short phy_sector_per_track;     // 24-25
    unsigned short n_heads;                  // 26-27
    unsigned int n_hidden_sectors;           // 28-31
    unsigned int n_sectors_32;               // 32-35
    unsigned int n_sector_per_fat_32;        // 36-39
    unsigned short mirror_flag;              // 40-41
    unsigned short version;                  // 42-43
    unsigned int first_cluster;              // 44-47
    unsigned short info;                     // 48-49
    char other[17];                 // 50-66
    unsigned int s_number;                   // 67-70
    char label[11];                 // 71-81
    char type[8];                   // 82-89

    unsigned short bytes_per_logic_sector;   // 11-12
    unsigned short n_root_dir_entries;       // 17-18

} __attribute__ ( ( packed ) ) fat32_boot_sector_t;

typedef struct fat32_dir
{
    char name[8];           // 0-7
    char ext[3];            // 8-10
    char attr[9];           // 11-19
    unsigned short cluster_high;  // 20-21
    unsigned int ext_attr;      // 22-25
    unsigned short cluster_low;   // 26-27
    unsigned int size;          // 28-31
} __attribute__ (( packed))fat32_dir_t;

typedef struct fat32_node
{
    char name[10];
    char ext[5];
    unsigned int cluster;
    unsigned int size;
} fat32_node_t;

void fat32_init ( );
void setup_fat32(struct filesystem **fs, char *name);

#endif