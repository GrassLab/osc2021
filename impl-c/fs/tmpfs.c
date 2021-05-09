#include "fs/tmpfs.h"
#include "fs/vfs.h"

#include "mm.h"
#include <stddef.h>
struct vnode_operations *tmpfs_v_ops = NULL;
struct file_operations *tmpfs_f_ops = NULL;

struct filesystem tmpfs = {
    .name = "tmpfs",
    .setup_mount = tmpfs_setup_mount,
};

// bind operations
int tmpfs_init() {
  tmpfs_v_ops = kalloc(sizeof(struct vnode_operations));
  tmpfs_v_ops->lookup = tmpfs_lookup;
  tmpfs_v_ops->create = tmpfs_create;

  tmpfs_f_ops = kalloc(sizeof(struct file_operations));
  tmpfs_f_ops->read = tmpfs_read;
  tmpfs_f_ops->write = tmpfs_write;
  return 0;
}

int tmpfs_setup_mount(struct filesystem *fs, struct mount *mount) {
  // TODO
  return 0;
}

int tmpfs_write(struct file *f, const void *buf, unsigned long len) {
  // TODO
  return 0;
}
int tmpfs_read(struct file *f, void *buf, unsigned long len) {
  // TODO
  return 0;
}
int tmpfs_lookup(struct vnode *dir_node, struct vnode **target,
                 const char *component_name) {
  // TODO
  return 0;
}
int tmpfs_create(struct vnode *dir_node, struct vnode **target,
                 const char *component_name) {
  // TODO
  return 0;
}
