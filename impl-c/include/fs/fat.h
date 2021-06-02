#pragma once

#include "fs/vfs.h"

// Would be called autolatically
int fat_init();

// Basic Operations
int fat_setup_mount(struct filesystem *fs, struct mount *mount);
int fat_write(struct file *f, const void *buf, unsigned long len);
int fat_read(struct file *f, void *buf, unsigned long len);
int fat_lookup(struct vnode *dir_node, struct vnode **target,
               const char *component_name);
int fat_create(struct vnode *dir_node, struct vnode **target,
               const char *component_name);

extern struct filesystem fat;

void fat_dev();

// Only used for running tests
void test_fat();