#ifndef FAT32_H
#define FAT32_H

#include "vfs.h"
#define BLOCK_SIZE 512
#define EOC 0xfffffff // Last cluster in file


typedef struct fat32_meta {
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short num_reserved_sector;
    unsigned char num_fat;
    unsigned int sectors_per_fat;
    unsigned int root_cluster_num;
} meta;

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