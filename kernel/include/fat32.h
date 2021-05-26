#ifndef _FAT32_H
#define _FAT32_H

#include "vfs.h"
#include "def.h"

struct file_info {
    char name[9];
    char ext[4];
    int size;
};

struct entry_info {
    int start_cluster;
    int dir_cluster;
    int entry_index;
};

struct fat32_internal {
    struct file_info f_info;
    struct entry_info e_info;

    struct vnode *l_child; // left most child
    struct vnode *r_sibling; // right next sibling
};



int fat32_setup(struct filesystem *fs, struct mount* mount);
int fat32_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
int fat32_create(struct vnode *dir_node, struct vnode **target, const char *component_name);
int fat32_write (struct file *file, const void *buf, size_t len);
int fat32_read (struct file *file, void *buf, size_t len);
void sd_init_fs(struct vnode *root);

struct vnode *create_fat32_vnode(struct vnode *parent, struct file_info *f_info, struct entry_info *e_info);
struct fat32_internal *create_fat32_vnode_internal(struct file_info *f_info, struct entry_info *e_info);
void fat32_update_size(struct vnode *node, int size);

#endif