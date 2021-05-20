# include "tmpfs.h"
# include "log.h"
# include "mem.h"
# include "flags.h"
# include "uart.h"

struct filesystem tmpfs = {
  .name = "tmpfs",
  .setup_mount = tmpfs_setup_mount,
};

struct vnode_operations tmpfs_file_v_ops{
  .lookup = vfs_lookup,
  .create = tmpfs_create,
  .mkdir = tmpfs_mkdir,
  .cat = tmpfs_file_cat,
};

struct vnode_operations tmpfs_dir_v_ops{
  .lookup = vfs_lookup,
  .create = tmpfs_create,
  .mkdir = tmpfs_mkdir,
  .cat = tmpfs_dir_cat,
};

struct file_operations tmpfs_f_ops{
  .write = tmpfs_write,
  .read = tmpfs_read,
};

struct filesystem* tmpfs_get_fs(){
  return &tmpfs;
  //mount_point->fs = &tmpfs;
  //mount_point->parent = parent;
  //str_copy(name, mount_point->name);
}

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount){
  //struct dentry *new_d = vfs_create_dentry(0, (char *)"/", DIR);
  struct dentry *new_d = vfs_create_dentry(mount->parent, mount->name, DIR);
  if (new_d == 0){
    return -1;
  }
  struct vnode *vnode = new_d->vnode;
  vnode->mount = mount;
  vnode->mode = TMPFS_DEFAULT_MODE;
  vnode->v_ops = &tmpfs_dir_v_ops;
  vnode->f_ops = &tmpfs_f_ops;
  vnode->internal = 0;
  vnode->dentry = new_d;
  vnode->file = 0;
  mount->root = vnode;
  return 0;
}

int tmpfs_mkdir(struct vnode* dir_node, struct vnode** target, const char* component_name){
  if ((dir_node->mode & F_WR) == 0){
    return -2;  // Can't write.
  }
  struct dentry *new_d = vfs_create_dentry(dir_node->dentry, component_name, DIR);
  if (new_d == 0){
    return -1;
  }
  struct vnode *vnode = new_d->vnode;
  vnode->mount = dir_node->mount;
  vnode->mode = TMPFS_DEFAULT_MODE;
  vnode->v_ops = &tmpfs_dir_v_ops;
  vnode->f_ops = &tmpfs_f_ops;
  vnode->internal = 0;
  vnode->dentry = new_d;
  vnode->file = 0;
  *target = vnode;
  return 0;
}

int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name){
  if ((dir_node->mode & F_WR) == 0){
    return -2;  // Can't write.
  }
  struct dentry *new_d = vfs_create_dentry(dir_node->dentry, component_name, FILE);
  if (new_d == 0){
    return -1;
  }
  struct vnode *vnode = new_d->vnode;
  vnode->mount = dir_node->mount;
  vnode->mode = TMPFS_DEFAULT_MODE;
  vnode->v_ops = &tmpfs_file_v_ops;
  vnode->f_ops = &tmpfs_f_ops;
  struct tmpfs_internal *internal = MALLOC(struct tmpfs_internal, 1);
  internal->size = 0;
  internal->content[0] = '\0';
  vnode->internal = internal;
  vnode->dentry = new_d;
  vnode->file = 0;
  *target = vnode;
  return 0;
}

int tmpfs_read(struct file* file, void* buf, size_t len){
  if (file->vnode->mode & F_RD == 0){
    log_puts("[WARNING] Read permission denied\n", WARNING);
    return -2;
  }
  int r = 0;
  struct tmpfs_internal *internal = (struct tmpfs_internal *) file->vnode->internal;
  if (file->f_pos >= internal->size){
    log_puts("[Error] TMPFS read file over size!\n", SEVERE);
    return -1;
  }
  for (int i = 0; i< len; i++){
    if (file->f_pos < internal->size && internal->content[file->f_pos] != '\0'){
      ((char*)buf)[i] = internal->content[file->f_pos];
      file->f_pos++;
      r++;
    }
    else{
      break;
    }
  }
  return r;
}

int tmpfs_write(struct file* file, const void* buf, size_t len){
  //log_puts("Enter Write\n", INFO);
  if (file->vnode->mode & F_WR == 0){
    log_puts("[WARNING] Write permission denied\n", WARNING);
    return -2;
  }
  int r = 0;
  struct tmpfs_internal *internal = (struct tmpfs_internal *) file->vnode->internal;
  if (internal->size >= TMPFS_MAX_SIZE){
    log_puts("[Error] TMPFS file over size!\n", SEVERE);
    return -1;
  }
  for (int i = 0; i< len; i++){
    if (internal->size < TMPFS_MAX_SIZE){
      internal->content[file->f_pos] = ((char *)buf)[i];
      file->f_pos++;
      //internal->size++;
      r++;
    }
    else{
      break;
    }
  }
  internal->size = file->f_pos;
  internal->content[internal->size] = '\0';
  return r;
}

int tmpfs_file_cat(struct vnode *vnode){
  struct tmpfs_internal *internal = (struct tmpfs_internal*)vnode->internal;
  uart_puts(internal->content);
  return 0;
}

int tmpfs_dir_cat(struct vnode *vnode){
  return -1;
}
