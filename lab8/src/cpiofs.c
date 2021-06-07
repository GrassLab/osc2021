# include "cpiofs.h"
# include "cpio.h"
# include "log.h"
# include "mem.h"
# include "flags.h"
# include "uart.h"
# include "my_string.h"

struct filesystem cpiofs = {
  .name = "cpiofs",
  .setup_mount = cpiofs_setup_mount,
  .unmount = cpiofs_unmount,
};

struct vnode_operations cpiofs_file_v_ops{
  .lookup = vfs_lookup,
  .create = cpiofs_create,
  .mkdir = cpiofs_mkdir,
  .cat = cpiofs_file_cat,
  .size = cpiofs_get_size,
  .rm = cpiofs_rm,
};

struct vnode_operations cpiofs_dir_v_ops{
  .lookup = vfs_lookup,
  .create = cpiofs_create,
  .mkdir = cpiofs_mkdir,
  .cat = cpiofs_dir_cat,
  .size = vfs_get_dir_size,
  .rm = cpiofs_rm,
};

struct file_operations cpiofs_f_ops{
  .write = cpiofs_write,
  .read = cpiofs_read,
};

struct filesystem* cpiofs_get_fs(){
  return &cpiofs;
}

static void set_file_vnode(struct mount *mount, struct dentry *new_d, int size, char *content){
  struct vnode *vnode = new_d->vnode;
  vnode->mount = mount;
  vnode->mode = CPIOFS_DEFAULT_MODE;
  vnode->v_ops = &cpiofs_file_v_ops;
  vnode->f_ops = &cpiofs_f_ops;
  //vnode->dentry = new_d;
  vnode->file = 0;
  vnode->internal = MALLOC(cpiofs_internal, 1);
  struct cpiofs_internal *internal = (struct cpiofs_internal *) vnode->internal;
  internal->size = size;
  internal->content = content;
}

static void set_dir_vnode(struct mount *mount, struct dentry *new_d){
  struct vnode *vnode = new_d->vnode;
  vnode->mount = mount;
  vnode->mode = CPIOFS_DEFAULT_MODE;
  vnode->v_ops = &cpiofs_dir_v_ops;
  vnode->f_ops = &cpiofs_f_ops;
  vnode->internal = 0;
  //vnode->dentry = new_d;
  vnode->file = 0;
}

int cpiofs_setup_mount(struct filesystem* fs, struct mount* mount){
  //struct dentry *new_d = vfs_create_dentry(0, (char *)"/", DIR);
  /*
  struct dentry *new_d = vfs_create_dentry(mount->parent, mount->name, DIR);
  if (new_d == 0){
    return -1;
  }
  */
  struct dentry *new_d = mount->root->dentry;
  set_dir_vnode(mount, new_d);
  //mount->root = new_d->vnode;
  int argc = cpio_get_argc();
  char *argv[argc+1];
  cpio_get_argv(argv);
  for (int i = 0; i<argc;i++){
    if (str_cmp(argv[i], (char *)".") == 1){
      continue;
    }
    int pathname_len = str_len(argv[i]);
    char dirname[pathname_len+1];
    str_copy(argv[i], dirname);
    char *filename = 0;
    for (int i = pathname_len-1; i >= 0; i--){
      if (dirname[i] == '/'){
        dirname[i] = '\0';
        filename = &dirname[i+1];
        break;
      }
    }
    struct vnode *dvnode = new_d->vnode;
    int dir_exist = 0;
    if (filename == 0){
      filename = dirname;
    }
    else if(dirname[0] == '\0'){
      dvnode = new_d->vnode;
    }
    else{
      struct vnode *tvnode = 0;
      dir_exist = get_vnode_by_path(dvnode, &tvnode, dirname);
      dvnode = tvnode;
    }
    // Check dir exist
    if (dir_exist != 0){
      return -1;
    }
    struct vnode *target;
    int lpr = dvnode->v_ops->lookup(dvnode, &target, filename);
    if (lpr == 0){
      return -1;
    }
    char *content = 0;
    int size = cpio_get_content(argv[i], &content);
    if (size == 0){
      struct dentry *new_dt = vfs_create_dentry(dvnode->dentry, filename, DIR);
      set_dir_vnode(mount, new_dt);
    }
    else{
      struct dentry *new_dt = vfs_create_dentry(dvnode->dentry, filename, FILE);
      set_file_vnode(mount, new_dt, size, content);
    }
  }
  return 0;
}

static int rm_vnode_recv(struct vnode *vnode){
  if (VISFILE(vnode)){
    if( cpiofs_file_release(vnode) ) return -1;
  }
  else if(VISDIR(vnode)){
    if (vnode->v_ops->size(vnode) <= 2){
      if( cpiofs_dir_release(vnode) ) return -1;
    }
    else{
      struct dentry *d = vnode->dentry;
      while(1){
        struct list_head *head_t = d->childs.prev;
        struct dentry *dt = container_of(head_t, struct dentry, list);
        if (dt->type == SDIR){
          break;
        }
        if( rm_vnode_recv(dt->vnode) ) return -1;
      }
    }
  }
  return 0;
}

int cpiofs_unmount(struct mount *mount){
  struct vnode *root = mount->root;
  return rm_vnode_recv(root);
}

int cpiofs_mkdir(struct vnode* dir_node, struct vnode** target, const char* component_name){
  log_puts((char *) "[Error] mkdir permission denied\n", WARNING);
  return -2;
}

int cpiofs_create(struct vnode* dir_node, struct vnode** target, const char* component_name){
  log_puts((char *) "[Error] Create permission denied\n", WARNING);
  return -2;
}

int cpiofs_read(struct file* file, void* buf, size_t len){
  if ((file->vnode->mode & F_RD) == 0){
    log_puts((char *) "[Error] Read permission denied\n", WARNING);
    return -2;
  }
  int r = 0;
  struct cpiofs_internal *internal = (struct cpiofs_internal *) file->vnode->internal;
  if (file->f_pos >= internal->size){
    log_puts((char *) "[SEVERE] CPIOFS read file over size!\n", SEVERE);
    return -1;
  }
  for (size_t i = 0; i<len; i++){
    //if (file->f_pos < internal->size && internal->content[file->f_pos] != '\0'){
    if (file->f_pos < internal->size){
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

int cpiofs_write(struct file* file, const void* buf, size_t len){
  //log_puts("Enter Write\n", INFO);
  log_puts((char *) "[Error] Write permission denied\n", WARNING);
  return -2;
}

int cpiofs_file_cat(struct vnode *vnode){
  struct cpiofs_internal *internal = (struct cpiofs_internal*)vnode->internal;
  uart_puts_n(internal->content, internal->size);
  return 0;
}

int cpiofs_dir_cat(struct vnode *vnode){
  return -1;
}

int cpiofs_get_size(struct vnode *vnode){
  if (vnode->dentry->type == FILE){
    struct cpiofs_internal *internal = (struct cpiofs_internal*)vnode->internal;
    return internal->size;
  }
  return 0;
}

int cpiofs_rm(struct vnode *vnode){
  log_puts((char *) "[Error] Items in CPIOFS can't br removed.\n", WARNING);
  return -1;
}

int cpiofs_dir_release(struct vnode *vnode){
  int size = vnode->v_ops->size(vnode);
  if (size > 2){
    log_puts((char *) "[Error] Dir is not empty.\n", WARNING);
    return -1;
  }
  struct dentry *d = vnode->dentry;
  list_del(&(d->list));
  while( !list_is_empty(&(d->list)) ){
    struct dentry *dt = container_of(d->childs.next, struct dentry, list);
    list_del(&(dt->list));
    log_puts((char *) "[INFO] Remove sdentry < ", INFO);
    log_puts(dt->name, INFO);
    log_puts((char *) " > .\n", INFO);
    free(dt);
  }
  log_puts((char *) "[INFO] Remove dentry < ", INFO);
  log_puts(d->name, INFO);
  log_puts((char *) " > .\n", INFO);
  free(d);
  free(vnode);
  return 0;
}

int cpiofs_file_release(struct vnode *vnode){
  if (vnode->file){
    vnode->file->vnode = 0;
  }
  struct dentry *d = vnode->dentry;
  list_del(&(d->list));
  log_puts((char *) "[INFO] Release file < ", INFO);
  log_puts(d->name, INFO);
  log_puts((char *) " > .\n", INFO);
  free(d);
  free(vnode->internal);
  free(vnode);
  return 0;
}
