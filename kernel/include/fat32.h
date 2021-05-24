#ifndef _FAT32_H
#define _FAT32_H

#include "vfs.h"
#include "def.h"

int fat32_setup(struct filesystem *fs, struct mount* mount);
int fat32_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
int fat32_create(struct vnode *dir_node, struct vnode **target, const char *component_name);
int fat32_write (struct file *file, const void *buf, size_t len);
int fat32_read (struct file *file, void *buf, size_t len);

#endif