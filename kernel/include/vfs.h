#pragma once

#include <stddef.h>

#define O_CREAT 1

typedef enum { FILE_DIRECTORY, FILE_REGULAR, FILE_DEVICE, FILE_NONE } FILE_TYPE;

struct vnode {
  struct mount* mount;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  void* internal;
};

struct file {
  struct vnode* vnode;
  size_t f_pos;  // The next read/write position of this opened file
  struct file_operations* f_ops;
  int flags;
};

struct mount {
  struct vnode* root;
  struct filesystem* fs;
};

struct filesystem {
  const char* name;
  int (*setup_mount)(struct filesystem* fs, struct mount* mount);
  struct filesystem* next;
};

struct filesystem_list {
  struct filesystem *head, *tail;
};

struct file_operations {
  int (*write)(struct file* file, const void* buf, size_t len);
  int (*read)(struct file* file, void* buf, size_t len);
  int (*list)(struct file* file, void* buf, int index);
};

struct vnode_operations {
  int (*lookup)(struct vnode* dir_node, struct vnode** target,
                const char* component_name);
  int (*create)(struct vnode* dir_node, struct vnode** target,
                const char* component_name, FILE_TYPE type);
  int (*set_parent)(struct vnode* child_node, struct vnode* parent_node);
};

struct filesystem_list fs_list;
struct mount* rootfs;
struct vnode* current_dir;

void vfs_test();
void vfs_ls_test();
void vfs_hard_test();

void vfs_init();
int register_filesystem(struct filesystem* fs);
struct filesystem* get_fs_by_name(const char* name);
int vfs_find_vnode(struct vnode** target, const char* pathname);
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, size_t len);
int vfs_read(struct file* file, void* buf, size_t len);
int vfs_list(struct file* file, void* buf, int index);
int vfs_mkdir(const char* pathname);
int vfs_chdir(const char* pathname);
int vfs_mount(const char* device, const char* mountpoint,
              const char* filesystem);
int vfs_umount(const char* mountpoint);
