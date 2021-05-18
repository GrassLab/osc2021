#ifndef _FAT32_H_
#define _FAT32_H_

#include <types.h>
#include <vfs.h>

#define FAT32_BLOCK_SIZE 512
#define FAT32_D_ENTRY_SIZE 32
#define FAT32_D_ENTRY_NAME_SIZE 8
#define FAT32_D_ENTRY_EXTENSION_SIZE 3

#define FAT32_EOC_MAX 0x0fffffff
#define FAT32_EOC_MIN 0x0ffffff8
#define IS_EOC(lba) (lba >= FAT32_EOC_MIN && lba <= FAT32_EOC_MAX)

#define FAT32_D_ENTRY_PER_D_TABLE 16

#define FAT32_ENTRY_PER_FAT_TABLE 128
struct partition_entry {
  char ignore1[4];
  char type_code; //0x0B or 0x0C
  char ignore2[3];
  uint32_t lba;
  uint32_t num_of_sectors;
} __attribute__ ((packed));

struct mbr {
  char boot_code[446];
  struct partition_entry partitions[4];
  char signature[2];
} __attribute__((packed));

struct boot_sector {
  char jump_instruction[3];
  char oem_name[8];
  //BIOS Parameter Block EPB
  uint16_t bytes_per_sector; // most 512 bytes
  uint8_t sectors_per_cluster; // Allowed values are 1, 2, 4, 8, 16, 32, 64, and 128
  uint16_t num_of_reserved_sectors; // usually 32 for FAT32
  uint8_t num_of_fats; // Almost always 2
  uint16_t max_root_entries; // for FAT12 or FAT16
  uint16_t num_of_sectors; // 0 for FAT32
  uint8_t media_descriptor;
  uint16_t sectors_per_fat; // 0 for FAT32
  //DOS 3.31 BPB
  uint16_t sectors_per_track;
  uint16_t num_of_head;
  uint32_t num_of_hidden_sectors;
  uint32_t num_of_sectors_large;
  //FAT32 Extended BIOS Parameter Block
  uint32_t sectors_per_fat_large_fat32;
  uint16_t flag;
  uint16_t version; // 0 
  uint32_t cluster_num_of_root_dir; // typically 2 
  uint16_t sector_num_of_fs_info_sector; // typically 1
  uint16_t sector_num_of_backup_sector; //  typically 6
  char reserved[12]; // should be 0
  uint8_t drvie_num;
  uint8_t various;
  uint8_t signature; // must be 0x28 or 0x29
  uint32_t volume_id;
  char volume_label[11];
  char system_identifier[8]; // always FAT32   
  //???

} __attribute__ ((packed));
  

struct directory_entry {
  char name[8];			
  char extension[3];
  char attribute; //0x10 sub-directory
  char ignore1[8];		
  uint16_t start_cluster_high;
  char ignore2[4];	
  uint16_t start_cluster_low;
  uint32_t size;	
} __attribute__ ((packed));

struct fs_information_sector {
  uint32_t signature1; // 0x52 0x52 0x61 0x41 = "RRaA"
  char reserved1[480];
  uint32_t signature2; // 0x72 0x72 0x41 0x61 = "rrAa"
  uint32_t num_of_free_data_clusters;
  uint32_t num_of_allocated_data_clusters; 
  char reserved2[12];
  uint32_t signature3; // 0x00 0x00 0x55 0xAA

};

struct directory_table {
  struct directory_entry *root_entry;
  size_t size;
};

struct fat32_info {
  struct partition_entry* p_entry;
  struct boot_sector *boot_sector;
  struct directory_table *d_table;
};

struct file_operations fat32_fops;

struct vnode_operations fat32_vops;

struct fat32_info fat32_info_list[4];
struct fat32_info *current_partition;
struct mbr _mbr;
//struct boot_sector* _boot_sectors[4];

void fat32_init();

void fat32_parse_mbr();
void* fat32_parse_boot_sector(uint32_t lba);
void fat32_parse_root_directory(struct fat32_info *fat32_info);
void fat32_traverse_root_directory(struct fat32_info* _fat32_info);
void test_read_file1(struct fat32_info * _fat32_info);

void* fat32_vnode_create(struct mount* _mount);
/*static int setup_mount(struct filesystem* fs, struct mount* _mount);
static int write(struct file* file, const void* buf, size_t len);
static int read(struct file* file, void* buf, size_t len);
static int lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
static int create(struct vnode* dir_node, struct vnode** target, const char* component_name);*/
#endif