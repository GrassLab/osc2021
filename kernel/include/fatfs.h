#pragma once

#include <stddef.h>
#include <stdint.h>

#include "utils.h"
#include "vfs.h"

#define BLOCK_SIZE 512
#define MBR_PARTITION_BASE 0x1BE
#define MAX_FILES_IN_DIR 16
#define FATFS_BUF_SIZE (10 * kb)

struct mbr_partition_entry {
  unsigned char status_flag;              // 0x0
  unsigned char partition_begin_head;     // 0x1
  unsigned short partition_begin_sector;  // 0x2
  unsigned char partition_type;           // 0x4
  unsigned char partition_end_head;       // 0x5
  unsigned short partition_end_sector;    // 0x6
  unsigned int starting_sector;           // 0x8
  unsigned int sector_count;              // 0xC
} __attribute__((packed));

// struct mbr_partition_entry {
//   char b[8];
//   unsigned short addr;
//   char c[6];
// };

// struct mbr {
//   unsigned char bootstrap_code[0x1BE];
//   struct mbr_partition_entry entries[4];
//   // unsigned char b[64];
//   unsigned char boot_signature[2];
// } __attribute__((packed));

struct fatfs_boot_sector {
  unsigned char bootjmp[3];                 // 0x00
  unsigned char oem_name[8];                // 0x08
  unsigned short bytes_per_sector;          // 0x0B
  unsigned char sectors_per_cluster;        // 0x0D
  unsigned short reserved_sector_count;     // 0x0E
  unsigned char fat_count;                  // 0x10
  unsigned short root_entry_count;          // 0x11
  unsigned short total_sectors;             // 0x13
  unsigned char media_descriptor;           // 0x15
  unsigned short sectors_per_fat;           // 0x16
  unsigned short sectors_per_track;         // 0x18
  unsigned short head_count;                // 0x1A
  unsigned int hidden_sector_count;         // 0x1C
  unsigned int total_sectors_32;            // 0x20
  unsigned int sectors_per_fat_32;          // 0x24
  unsigned short mirror_flags;              // 0x28
  unsigned short fat_version;               // 0x2A
  unsigned int root_cluster;                // 0x2C
  unsigned short info_sector_number;        // 0x30
  unsigned short backup_boot_sector_count;  // 0x32
  unsigned char reserved_0[12];             // 0x34
  unsigned char drive_number;               // 0x40
  unsigned char reserved_1;                 // 0x41
  unsigned char boot_signature;             // 0x42
  unsigned int volume_id;                   // 0x43
  unsigned char volume_label[11];           // 0x47
  unsigned char fat_type_label[8];          // 0x52
} __attribute__((packed));

struct fatfs_dentry {
  unsigned char filename[8];                // 0x00
  unsigned char extension[3];               // 0x08
  uint8_t file_attr;                        // 0x0B
  unsigned char reserved;                   // 0x0C
  uint8_t created_time_ms;                  // 0x0D
  unsigned short created_time;              // 0x0E
  unsigned short created_date;              // 0x10
  unsigned short last_modified_date_stamp;  // 0x12
  unsigned short cluster_high;              // 0x14
  unsigned short last_modified_time;        // 0x16
  unsigned short last_modified_date;        // 0x18
  unsigned short cluster_low;               // 0x1A
  unsigned int file_size;                   // 0x1C
} __attribute__((packed));

struct fatfs_buf {
  int flag;
  size_t size;
  char buffer[FATFS_BUF_SIZE];
};

struct fatfs_fentry {
  char name[20];
  int name_len;  // test1.txt -> 5 (before .)
  int starting_cluster;
  FILE_TYPE type;
  struct vnode* vnode;
  struct vnode* parent_vnode;
  struct fatfs_fentry* child[MAX_FILES_IN_DIR];
  struct fatfs_buf* buf;
};

struct fatfs_boot_sector* fat_boot_sector;
struct fatfs_dentry* fat_root_dentry;
int fat_starting_sector;
int data_starting_sector;
int root_starting_sector;

struct vnode_operations* fatfs_v_ops;
struct file_operations* fatfs_f_ops;

void fatfs_init();
void fatfs_set_fentry(struct fatfs_fentry* fentry, FILE_TYPE type,
                      struct vnode* vnode, int starting_cluster);
int fatfs_setup_mount(struct filesystem* fs, struct mount* mount);
int fatfs_lookup(struct vnode* dir_node, struct vnode** target,
                 const char* component_name);
int fatfs_set_parent(struct vnode* child_node, struct vnode* parent_vnode);
int fatfs_write(struct file* file, const void* buf, size_t len);
int fatfs_read(struct file* file, void* buf, size_t len);
int fatfs_list(struct file* file, void* buf, int index);
