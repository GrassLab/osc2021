#ifndef _FAT32_H
#define _FAT32_H

#include "types.h"
#include "vfs.h"
#include "printf.h"

#define BLOCK_SIZE                          512
#define FAT32_ENTRY_SIZE                    4      // FAT table entry size in bytes
#define FAT32_ENTRY_PER_BLOCK               (BLOCK_SIZE / FAT32_ENTRY_SIZE)

#define FAT32_DIRECTORY_ENTRY_SIZE          4      // Regular Directory Entry Size in bytes
#define FAT32_DIRECTORY_ENTRY_PER_BLOCK     (BLOCK_SIZE / FAT32_DIRECTORY_ENTRY_SIZE)

#define DIRECTORY_ENTRY_ATTR_DIRECTORY      0x10

#define INVALID_CLUSTER_INDEX               0       // 0 and 1 is not a valid value, We also use it as a error code here. 
#define FAT32_ERROR -1

// end-of-cluster-chain marker (typically 0x0FFFFFFF or 0x0FFFFFF8 on FAT32)
// For spec, EOC can be 0x?FFFFFF8 -0x?FFFFFFF. In our lab, EOC is 0x0FFFFFFF 
#define EOC_FILTER 0x0FFFFFFF 

struct fat32_boot_sector 
{
    uint8_t  bootjmp[3];                        // 0x0
    uint8_t  oem_name[8];                       // 0x3

    // BIOS Parameter Block
    uint16_t bytes_per_logical_sector;          // 0xB-0xC
    uint8_t  sector_per_cluster;                // 0xD
    uint16_t nr_reserved_sectors;               // 0xE-0xF
    uint8_t  nr_fat_table;                      // 0x10
    uint16_t nr_max_root_dir_entries_16;        // 0x11-0x12
    uint16_t nr_logical_sectors_16;             // 0x13-0x14
    uint8_t  media_descriptor;                  // 0x15
    uint16_t logical_sector_per_fat_16;         // 0x16-0x17
    uint16_t physical_sector_per_track;         // 0x18-0x19
    uint16_t nr_heads;                          // 0x1A-0x1B
    uint32_t nr_hidden_sectors;                 // 0x1C-0x1F
    uint32_t nr_sectors_32;                     // 0x20-0x23

    // FAT32 Extended BIOS Parameter Block (EBPB)
    uint32_t nr_sectors_per_fat_32;             // 0x24-0x27
    uint16_t mirror_flag;                       // 0x28-0x29
    uint16_t version;                           // 0x2A-0x2B
    uint32_t root_dir_start_cluster_num;        // 0x2C-0x2F
    uint16_t fs_info_sector_num;                // 0x30-0x31
    uint16_t boot_sector_bak_first_sector_num;  // 0x32-0x33
    uint32_t reserved[3];                       // 0x34-0x3F
    uint8_t  physical_drive_num;                // 0x40
    uint8_t  unused;                            // 0x41
    uint8_t  extended_boot_signature;           // 0x42
    uint32_t volume_id;                         // 0x43-0x46
    uint8_t  volume_label[11];                  // 0x47-0x51
    uint8_t  fat_system_type[8];                // 0x52-0x59
} __attribute__((packed));

/**
 * Important metadata for fat32
 * In our lab, block idx is equal to sector idx (Because block size equal to sector size here)
 */
struct fat32_metadata
{
    uint32_t fat_region_blk_idx;    // Fat region block idx
    uint32_t data_region_blk_idx;   // data region block idx
    uint32_t rootDir_first_cluster; // block idx of first cluster of root directory, Often this field is set to 2. (Often first cluster number of all cluster)
    uint8_t  sector_per_cluster;
    uint32_t nr_fat;
    uint32_t sector_per_fat;
};

// Struct of fat32 directory entry. Short Filenames(SFN) version
struct fat32_dirEnt {
    uint8_t name[8];            // 0x0-0x7.   File name: 8 ASCII characters, padded with spaces. If the file name starts with 0x00, the previous entry was the last entry.
    uint8_t ext[3];             // 0x8-0xA.   File extension
    uint8_t attr;               // 0xB.       Attributes of the file
    uint8_t reserved;           // 0xC.       Reserved
    uint8_t create_time[3];     // 0xD-0xF.
    uint16_t create_date;       // 0x10-0x11.
    uint16_t last_access_date;  // 0x12-0x13.
    uint16_t cluster_high;      // 0x14-0x15.
    uint32_t ext_attr;          // 0x16-0x19. 0x16 time last write,  0x18 date last write                                    
    uint16_t cluster_low;       // 0x1A-0x1B.
    uint32_t size;              // 0x1C-0x1F. The size of the file in bytes.
} __attribute__((packed));

/**
 * Internal struct for each fat32 vnode (vnode type either directory or regular file)
 */
struct fat32_internal
{
    uint32_t first_cluster;                    // Idx of first block(sector) of cluster 
    uint32_t dirEntry_blk_idx;                 // Idx of block(sector) of direcotry entry 
    uint32_t size;                             // The size of the file in bytes.
};


// For sd card fat32 metadata
extern struct fat32_metadata fat32_metadata; 


int fat32_setup_mount(struct filesystem* fs, struct mount* mount, const char *component_name);
int fat32_register();
struct dentry* fat32_create_dentry(struct dentry *parent, const char *name, int type);
struct vnode* fat32_create_vnode(struct dentry *dentry, int type);

int fat32_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
int fat32_write(struct file *file, const void *buf, size_t len);
int fat32_read(struct file *file, void *buf, size_t len);
int fat32_create(struct vnode *dir_node, struct vnode **target, const char *component_name);
int fat32_mkdir(struct vnode *parent, const char *component_name);

static inline void _dump_fat32_metadata(struct fat32_metadata *fat32_meta)
{
    printf("============dump fat32_metadata============\n");
    printf("fat_region_blk_idx : %u\n",    fat32_meta->fat_region_blk_idx);   // Fat region block idx
    printf("data_region_blk_idx : %u\n",   fat32_meta->data_region_blk_idx);   // data region block idx
    printf("rootDir_first_cluster : %u\n",   fat32_meta->rootDir_first_cluster); // block idx of first cluster of root directory
    printf("sector_per_cluster : %u\n",      fat32_meta->sector_per_cluster);
    printf("nr_fat : %u\n",                  fat32_meta->nr_fat);
    printf("sector_per_fat : %u\n",          fat32_meta->sector_per_fat);
}

static inline void _dump_fat32_internal(struct fat32_internal *internal)
{
    printf("============dump fat32_internal============\n");
    printf("first_cluster : %u\n",                  internal->first_cluster);
    printf("DirEntry_first_cluster_blk_idx : 0x%x\n", internal->dirEntry_blk_idx);
    printf("size : %u\n",                           internal->size);
}

#endif