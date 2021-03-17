#include "vfs.h"

#include "io.h"
#include "mem.h"
#include "util.h"

struct super_block *rootfs;

void init_rootfs(struct super_block *sb) {
  rootfs = sb;
  sb->mount_id = 0;
}

int vfs_mkdir(const char *path, struct dentry *dir) {
  return (*(dir->node->sb->sb_ops.mkdir))(path, dir);
}
int vfs_chdir(const char *path, struct dentry *dir) {
  return (*(dir->node->sb->sb_ops.chdir))(path, dir);
}

int vfs_rmdir(const char *path, struct dentry *dir) {
  return (*(dir->node->sb->sb_ops.rmdir))(path, dir);
}

int vfs_open(const char *path, struct dentry *dir, struct file *f) {
  return (*(dir->node->sb->sb_ops.open))(path, dir, f);
}

int vfs_read(struct file *f, char *buf, size_t len) {
  return (*(f->file_node->sb->sb_ops.read))(f, buf, len);
}

int vfs_write(struct file *f, const char *buf, size_t len) {
  return (*(f->file_node->sb->sb_ops.write))(f, buf, len);
}

int vfs_close(struct file *f) { return (*(f->file_node->sb->sb_ops.close))(f); }

int vfs_get_dir(const char *path, struct dentry *dir) {
  if (dir->node == NULL) {
    return (*rootfs->sb_ops.get_dir)(path, dir, rootfs);
  } else {
    return (*(dir->node->sb->sb_ops.get_dir))(path, dir, dir->node->sb);
  }
}
int vfs_close_dir(struct dentry *dir) {
  return (*(dir->node->sb->sb_ops.close_dir))(dir);
}

void init_dentry(struct dentry *dir) { dir->node = NULL; }