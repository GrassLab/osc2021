#ifndef FAT32_H
#define FAT32_H

#include "vfs.h"

#define BLOCK_SIZE 			512
#define MBR_PARTITION_BASE 	0x1BE
#define MAX_FILES_IN_DIR 	16
#define FATFS_BUF_SIZE 		(10 * 1024)

// https://en.wikipedia.org/wiki/Master_boot_record
// The struct of partition of Master Boot Record (MBR) for disk
struct mbr_partition_entry 
{
	unsigned char status_flag;				// 0x0
	unsigned char partition_begin_head;		// 0x1
	unsigned short partition_begin_sector;  // 0x2
	unsigned char partition_type; 			// 0x4
	unsigned char partition_end_head;		// 0x5
	unsigned short partition_end_sector;	// 0x6
	unsigned int starting_sector;			// 0x8
	unsigned int sector_count;				// 0xC
} __attribute__((packed));

// https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system
// Boot Sector
struct fatfs_boot_sector 
{
	unsigned char bootjmp[3];				// 0x00
	unsigned char oem_name[8];				// 0x08
	
	// https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#BPB
	// BIOS Parameter Block
	unsigned short bytes_per_sector; 		// 0x0B-0x0C
	unsigned char sectors_per_cluster;		// 0x0D
	unsigned short reserved_sector_count;	// 0x0E-0x0F
	unsigned char fat_count;				// 0x10
	unsigned short root_entry_count;		// 0x11-0x12
	unsigned short total_sectors;			// 0x13-0x14
	unsigned char media_descriptor;			// 0x15
	unsigned short sectors_per_fat;			// 0x16-0x17
	unsigned short sectors_per_track;		// 0x18-0x19
	unsigned short head_count; 				// 0x1A-0x1B
	unsigned int hidden_sector_count;		// 0x1C-0x1F
	unsigned int total_sectors_32;			// 0x20-0x23
	
	//https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#EBPB
	//Extended BIOS Parameter Block
	unsigned int sectors_per_fat_32;		// 0x24-0x27
	unsigned short mirror_flags;			// 0x28-0x29
	unsigned short fat_version;				// 0x2A-0x2B
	unsigned int root_cluster;				// 0x2C-0x2F
	unsigned short info_sector_number;		// 0x30-0x31
	unsigned short backup_boot_sector_count;// 0x32-0x33
	unsigned char reserved_0[12];			// 0x34-0x3F
	unsigned char drive_number;				// 0x40
	unsigned char reserved_1;				// 0x41
	unsigned char boot_signature; 			// 0x42
	unsigned int volume_id;					// 0x43-0x46
	unsigned char volume_label[11];			// 0x47-0x51
	unsigned char fat_type_label[8];		// 0x52-0x59
} __attribute__((packed));

// https://en.wikipedia.org/wiki/8.3_filename
// Struct of fat32 directory entry. Short Filenames(SFN) version
struct fatfs_dir_entry 
{
	unsigned char filename[8];				// 0x00-0x07, File name: 8 ASCII characters
	unsigned char extension[3];				// 0x08-0x0A, File extension
	unsigned char attributes;				// 0x0B, Attributes of the file
	unsigned char reserved;					// 0x0C
	unsigned char created_time_ms; 			// 0x0D
	unsigned short created_time;			// 0x0E-0x0F
	unsigned short created_date;			// 0x10-0x11
	unsigned short last_access_date;		// 0x12-0x13
	unsigned short cluster_high;			// 0x14-0x15
	unsigned short last_modified_time;		// 0x16-0x17
	unsigned short last_modified_date;		// 0x18-0x19
	unsigned short cluster_low;				// 0x1A-0x1B
	unsigned int file_size;					// 0x1C-0x1F. The size of the file in bytes.
} __attribute__((packed));

struct fatfs_buf 
{
	int flag;
	unsigned int size;
	char buffer[FATFS_BUF_SIZE];
};

struct fatfs_entry 
{
	char name[20];
	int name_len;  // test1.txt -> 5 (before .)
	int starting_cluster;
	enum FILE_TYPE type;
	struct vnode* vnode;
	struct vnode* parent_vnode;
	struct fatfs_entry* child[MAX_FILES_IN_DIR];
	struct fatfs_buf* buf;
};

void fatfs_init();
int fatfs_setup_mount(struct filesystem* fs, struct mount* mount);
struct mbr_partition_entry* parse_mbr_metadata();
int get_starting_sector(int cluster);
void fatfs_set_directory(struct fatfs_entry* entry, struct fatfs_dir_entry* dentry);
void fatfs_set_entry(struct fatfs_entry* entry, enum FILE_TYPE type, struct vnode* vnode, int starting_cluster, int buf_size);
int fatfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
int fatfs_set_parent(struct vnode* child_node, struct vnode* parent_vnode);
int fatfs_write(struct file* file, const void* buf, unsigned int len);
int fatfs_read(struct file* file, void* buf, unsigned int len);
int fatfs_list(struct file* file, void* buf, int index);

#endif