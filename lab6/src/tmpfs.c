# include "tmpfs.h"
# include "log.h"

struct filesystem tmpfs = {
  .name = "tmpfs",
  .setup_mount = tmpfs_setup_mount,
};

struct vnode_operations tmpfs_v_ops{
  .lookup = 0,
  .create = 0,
  .mkdir = tmpfs_mkdir,
};

struct file_operations tmpfs_f_ops{
  .write = 0,
  .read = 0,
};

void tmpfs_get_mount_fs(struct mount* mount_point){
  mount_point->fs = &tmpfs;
}

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount){
  struct dentry *new_d = vfs_create_dentry(0, (char *)"/", DIR);
  if (new_d == 0){
    return -1;
  }
  struct vnode *vnode = new_d->vnode;
  vnode->mount = mount;
  vnode->mode = TMPFS_DEFAULT_MODE;
  vnode->v_ops = &tmpfs_v_ops;
  vnode->f_ops = &tmpfs_f_ops;
  vnode->internal = 0;
  vnode->dentry = new_d;
  mount->root = vnode;
  return 0;
}

int tmpfs_mkdir(struct vnode* dir_node, struct vnode** target, const char* component_name){
  if ((dir_node->mode & 2) == 0){
    return -2;  // Can't write.
  }
  struct dentry *new_d = vfs_create_dentry(dir_node->dentry, component_name, DIR);
  if (new_d == 0){
    return -1;
  }
  struct vnode *vnode = new_d->vnode;
  vnode->mount = dir_node->mount;
  vnode->mode = TMPFS_DEFAULT_MODE;
  vnode->v_ops = &tmpfs_v_ops;
  vnode->f_ops = &tmpfs_f_ops;
  vnode->internal = 0;
  vnode->dentry = new_d;
  *target = vnode;
  return 0;
}
