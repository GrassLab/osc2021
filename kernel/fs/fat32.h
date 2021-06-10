#ifndef FAT32_H
#define FAT32_H

#include "vfs.h"

#define FAT_BLOCK_SIZE 512

struct fat32_internel {
    char name[12];
    int size;

    int start_cluster;
    int entry_index;

    struct vnode * next_sibling;
};

int fat32_setup(struct filesystem * fs, struct mount * mount);
int fat32_lookup(struct vnode * dir_node, struct vnode ** target, const char * component_name);
int fat32_create(struct vnode * dir_node, struct vnode ** target, const char * component_name);
int fat32_read(struct file * file, void * buf, size_t len);
int fat32_write(struct file * file, const void * buf, size_t len);

struct vnode * fat32_create_vnode(struct vnode * src, const char * name, int size, int start_cluster, int entry_index);
struct fat32_internel * fat32_create_internel(const char * name, int size, int start_cluster, int entry_index);

void fat32_append_child(struct vnode * parent, struct vnode * child);
void fat32_update_filesize(struct vnode * node, int size);

void sd_init_fs(struct vnode * root);

#endif