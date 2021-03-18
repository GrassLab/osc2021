#ifndef VFS_H
#define VFS_H

#include <stddef.h>

// #define INITRAMFS 0x8000000

// void init_fs();
// void *get_root_dir();

struct inode {
  struct super_block *sb;
  int ref_count;
  void *internal;
};

struct dentry {
  struct inode *node;
};

struct file {
  struct inode *file_node;
  // struct inode *dir_node;
};

struct super_block_operations {
  int (*mkdir)(const char *path, struct dentry *dir);
  int (*chdir)(const char *path, struct dentry *dir);
  int (*rmdir)(const char *path, struct dentry *dir);

  int (*open)(const char *path, struct dentry *dir, struct file *f);
  int (*read)(struct file *f, char *buf, size_t len);
  int (*write)(struct file *f, const char *buf, size_t len);
  int (*close)(struct file *f);

  int (*get_dir)(const char *path, struct dentry *dir, struct super_block *sb);
  int (*close_dir)(struct dentry *dir);
};

struct super_block {
  int mount_id;
  struct super_block *parent_block;
  const char *fs_name;
  struct mount_list *mnt_l;
  struct super_block_operations sb_ops;
  void *internal;
};

struct vfsmount {
  int mount_id;
  struct super_block *sb;
  char *mount_path;
};

struct mount_list {
  struct vfsmount mount;
  struct mountkist *next;
};

void init_rootfs(struct super_block *sb);

int vfs_mkdir(const char *path, struct dentry *dir);
int vfs_chdir(const char *path, struct dentry *dir);
int vfs_rmdir(const char *path, struct dentry *dir);

int vfs_open(const char *path, struct dentry *dir, struct file *f);
int vfs_read(struct file *f, char *buf, size_t len);
int vfs_write(struct file *f, const char *buf, size_t len);
int vfs_close(struct file *f);

int vfs_get_dir(const char *path, struct dentry *dir);
int vfs_close_dir(struct dentry *dir);

void init_dentry(struct dentry *dir);

#endif