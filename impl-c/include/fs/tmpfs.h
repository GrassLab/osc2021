#pragma once

#include "fs/vfs.h"

/**
 * tmp fs: a temporary file system based on in-memory cpio fs
 **/

int tmpfs_init();

// make a node to be initramfs
int tmpfs_init_node();

// Basic file operations
int tmpfs_write(struct file *f, const void *buf, unsigned long len);
int tmpfs_read(struct file *f, void *buf, unsigned long len);
int tmpfs_lookup(struct vnode *dir_node, struct vnode **target,
                 const char *component_name);
int tmpfs_create(struct vnode *dir_node, struct vnode **target,
                 const char *component_name);

int tmpfs_setup_mount(struct filesystem *fs, struct mount *mount);

extern struct filesystem tmpfs;