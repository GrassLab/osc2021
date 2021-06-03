#ifndef FAT32_H
#define FAT32_H

#include "vfs.h"
#define BLOCK_SIZE 512
#define DENTRY_SIZE 32
#define EOC 0xfffffff // Last cluster in file

typedef struct fat32_meta {
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short num_reserved_sector;
    unsigned char num_fat;
    unsigned int sectors_per_fat;
    unsigned int root_cluster_num;
} meta;


struct boot_sector
{
  char ignore1[0xb];
  // I think there is alignment problem exist, so we separate the offset of 0xb into half
  unsigned char bytes_per_sector_lo; // 0xb
  unsigned char bytes_per_sector_hi; // 0xc
  
  unsigned char sectors_per_cluster; // 0xd
  unsigned short count_of_reserved;	//0xe
  unsigned char num_of_fat;	// 0x10
  char ignore2[0x13];
  unsigned int sectors_per_fat;	// 0x24
  char ignore3[4];
  unsigned int cluster_num_of_root;	// 0x2c
} __attribute__ ((packed));

struct dir_entry_data // 32 bytes
{
  char name[8];			// 0x00
  char extension[3];		// 0x08
  char ignore[9];		// 0x0b
  unsigned short start_hi;	// 0x14
  char ignore2[4];		// 0x16
  unsigned short start_lo;	// 0x1a
  unsigned int size;		// 0x1c
} __attribute__ ((packed));

typedef struct dir_entry {
    vnode *vnode;
    char *name; // Short file name and Short file extension (0x00 and 0x08)
    unsigned char attribute; // File Attributes 0x0b
    unsigned int size; // File size in byte 0x1c
    unsigned int cluster_index; // Start of file in clusters (Low two bytes in 0x1a; high two bytes in 0x14.)

    unsigned int parent_cluster_index; // for updating the size
} dentry;

struct fat32_meta fat32_meta;

int fat32_write(file* file, const void* buf, unsigned long len);
int fat32_read(file* file, void* buf, unsigned long len);
int fat32_setup(filesystem* fs, mount* mnt);
int fat32_lookup(vnode* dir_node, vnode** target, const char* component_name);


#endif