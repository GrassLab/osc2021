#pragma once
#include "list.h"
#include <stddef.h>
/**
 * A vnode could be mounted by another vnode with whole diffrent file system
 * tree, if mnt!=NULL, this vnode is mounted
 * */
struct vnode {
  struct mount *mnt;
  struct vnode_operations *v_ops;
  struct file_operations *f_ops;
  void *internal;
};

struct file {
  struct vnode *node;
  unsigned long f_pos; // The next read/write position of this opened file
  struct file_operations *f_ops;
  int flags;
};

struct mount {
  struct vnode *root;
  struct filesystem *fs;
};

struct filesystem {
  const char *name;

  // Method to call while mounting a node in vfs within this filesystem
  int (*setup_mount)(struct filesystem *fs, struct mount *mnt);

  // Available file systems will be linked inside os after initialized
  struct filesystem *next;
};

struct file_operations {
  int (*write)(struct file *f, const void *buf, unsigned long len);
  int (*read)(struct file *f, void *buf, unsigned long len);
};

struct vnode_operations {
  int (*lookup)(struct vnode *dir_node, struct vnode **target,
                const char *component_name);
  int (*create)(struct vnode *dir_node, struct vnode **target,
                const char *component_name);
};

/**
 * Rootfs is designed to be a mount point
 * */
extern struct mount *rootfs;

// Select a specific fs impl as rootfs
int mount_root_fs(const char *fs_impl);

// register the file system to the kernel.
int register_filesystem(struct filesystem *fs);

// 1. Lookup pathname from the root vnode.
// 2. Create a new file descriptor for this vnode if found.
// 3. Create a new file if O_CREAT is specified in flags.
struct file *vfs_open(const char *pathname, int flags);

// 1. release the file descriptor
int vfs_close(struct file *file);

// 1. write len byte from buf to the opened file.
// 2. return written size or error code if an error occurs.
int vfs_write(struct file *file, const void *buf, size_t len);

// 1. read min(len, readable file data size) byte to buf from the opened file.
// 2. return read size or error code if an error occurs.
int vfs_read(struct file *file, void *buf, size_t len);

void vfs_init();

// Only used for running tests
void test_vfs();

#define FILE_O_CREAT (1 << 0)

/**
 * @brief Get the first component name by returning it's start/end index pair in
 * `path`
 * @warning: `path[start_idx,end_idx]` does not contain '\0'
 * @param start_idx return the start index
 * @param end_idx return the end index(inclusive)
 * @retval 0 for succeed, -1 for failed
 */
int get_component(const char *path, /* Return*/ int *start_idx,
                  /* Return*/ int *end_idx);