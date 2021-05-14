# include "tmpfs.h"

struct filesystem tmpfs = {
  .name = "tmpfs",
  .setup_mount = tmpfs_setup_mount,
};

int tmpfs_get_mount_fs(struct mount* mount_point){
  mount_point->fs = &tmpfs;
}

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount){
  struct dentry *new_d = vfs_create_dentry(0, (char *)"/", DIR);
  struct vnode *vnode = new_d->vnode;
  vnode->mount = mount;
  vnode->mode = TMPFS_DEFAULT_MODE;
  //vnode->v_ops = tmpfs_v_ops;
  //vnode->f_ops = tmpfs_f_ops;
  vnode->internal = 0;
  mount->root = vnode;
  return 0;
}
