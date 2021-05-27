# include "fat32fs.h"
# include "log.h"
# include "mem.h"
# include "flags.h"
# include "uart.h"
# include "my_string.h"
# include "my_math.h"

struct filesystem fat32fs = {
  .name = "fat32fs",
  .setup_mount = fat32fs_setup_mount,
  .unmount = fat32fs_unmount,
};

struct vnode_operations fat32fs_file_v_ops{
  .lookup = vfs_lookup,
  .create = fat32fs_create,
  .mkdir = fat32fs_mkdir,
  .cat = fat32fs_cat,
  .size = fat32fs_get_size,
  .rm = fat32fs_rm,
};

struct vnode_operations fat32fs_dir_v_ops{
  .lookup = vfs_lookup,
  .create = fat32fs_create,
  .mkdir = fat32fs_mkdir,
  .cat = fat32fs_cat,
  .size = vfs_get_dir_size,
  .rm = fat32fs_rm,
};

struct file_operations fat32fs_f_ops{
  .write = fat32fs_write,
  .read = fat32fs_read,
};

struct filesystem* fat32fs_get_fs(){
  return &fat32fs;
}

static void set_file_vnode(struct mount *mount, struct dentry *new_d, uint32_t first_cluster,  int size, int dir_t_no){
  struct vnode *vnode = new_d->vnode;
  vnode->mount = mount;
  vnode->mode = FAT32FS_DEFAULT_MODE;
  vnode->v_ops = &fat32fs_file_v_ops;
  vnode->f_ops = &fat32fs_f_ops;
  vnode->file = 0;
  vnode->internal = MALLOC(fat32fs_internal, 1);
  struct fat32fs_internal *internal = (struct fat32fs_internal *) vnode->internal;
  internal->first_cluster = first_cluster;
  internal->cur_cluster = first_cluster;
  internal->cur_cluster_no = 0;
  internal->buf = 0;
  internal->dir_cluster_data = 0;
  internal->dir_t_no = dir_t_no;
  internal->size = size;
}

static void set_dir_vnode(struct mount *mount, struct dentry *new_d, uint32_t first_cluster, int dir_t_no){
  struct vnode *vnode = new_d->vnode;
  vnode->mount = mount;
  vnode->mode = FAT32FS_DEFAULT_MODE;
  vnode->v_ops = &fat32fs_dir_v_ops;
  vnode->f_ops = &fat32fs_f_ops;
  vnode->file = 0;
  vnode->internal = MALLOC(struct fat32fs_internal, 1);
  struct fat32fs_internal *internal = (struct fat32fs_internal *) vnode->internal;
  internal->first_cluster = first_cluster;
  internal->cur_cluster = first_cluster;
  internal->cur_cluster_no = 0;
  internal->buf = 0;
  internal->dir_cluster_data = 0;
  internal->dir_t_no = dir_t_no;
  internal->size = 0;
}

static void mount_log(char *filename, uint32_t first_cluster){
  log_puts(filename, INFO);
  log_puts(" . \tAt cluster : ", INFO);
  char ct[20];
  int_to_str(first_cluster, ct);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
}

static void load_dir_files(struct vnode *vnode, struct mount* mount){
  struct fat32fs_internal *interval = (struct fat32fs_internal *) vnode->internal;
  list_head list;
  struct cluster_data *cluster_data;
  get_fat32_dir_list(interval->first_cluster, &list, &cluster_data);
  interval->dir_cluster_data = cluster_data;
  list_head *pos;
  list_for_each(pos, &list){
    struct fat32_file_info *t = container_of(pos, struct fat32_file_info, list);
    char *filename = t->name;
    if (t->type == DIR){
      if(str_cmp(filename, ".") == 1){
        continue;
      }
      if(str_cmp(filename, "..") == 1){
        continue;
      }
      log_puts("Get <DIR>  : ", INFO);
      mount_log(filename, t->first_cluster);
      struct dentry *new_dt = vfs_create_dentry(vnode->dentry, filename, DIR);
      set_dir_vnode(mount, new_dt, t->first_cluster, t->dir_t_no);
      load_dir_files(new_dt->vnode, mount);
    }
    else{
      log_puts("Get <FILE> : ", INFO);
      mount_log(filename, t->first_cluster);
      struct dentry *new_dt = vfs_create_dentry(vnode->dentry, filename, FILE);
      set_file_vnode(mount, new_dt, t->first_cluster, t->size, t->dir_t_no);
    }
  }
  while( !list_is_empty(&list) ){
    list_head *ht = list.next;
    struct fat32_file_info *info = container_of(ht, struct fat32_file_info, list);
    list_del(ht);
    free(info->name);
    free(info);
  }
}

int fat32fs_setup_mount(struct filesystem* fs, struct mount* mount){
  struct dentry *new_d = mount->root->dentry;
  set_dir_vnode(mount, new_d, 2, -1);
  load_dir_files(new_d->vnode, mount);
  return 0;
}

static int rm_vnode_recv(struct vnode *vnode){
  if (VISFILE(vnode)){
    if( fat32fs_file_release(vnode) ) return -1;
  }
  else if(VISDIR(vnode)){
    if (vnode->v_ops->size(vnode) <= 2){
      if( fat32fs_dir_release(vnode) ) return -1;
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

int fat32fs_unmount(struct mount *mount){
  //log_puts("[Oops] Not implement yet.\n", WARNING);
  //return -1;
  struct vnode *root = mount->root;
  return rm_vnode_recv(root);
}

int fat32fs_mkdir(struct vnode* dir_node, struct vnode** target, const char* component_name){
  log_puts("[Oops] Not implemented mkdir yet.\n", WARNING);
  return -1;
}

int fat32fs_create(struct vnode* dir_node, struct vnode** target, const char* component_name){
  log_puts("[Oops] Not implemented create file yet.\n", WARNING);
  return -1;
}

static char fat32_get_char(struct fat32fs_internal *internal, size_t pos){
  uint32_t pos_h = pos/SECTOR_SIZE;
  uint32_t pos_l = pos%SECTOR_SIZE;
  if (internal->cur_cluster_no == pos_h){
    if (internal->buf == 0){
      internal->buf = MALLOC(char, SECTOR_SIZE);
      get_cluster(internal->cur_cluster, internal->buf);
    }
    return internal->buf[pos_l];
  }
  uint32_t cluster_no = (internal->cur_cluster_no > pos_h) ? 0 : internal->cur_cluster_no;
  uint32_t cluster_tmp = (internal->cur_cluster_no > pos_h) ? internal->first_cluster : internal->cur_cluster;
  while(cluster_no < pos_h){
    cluster_tmp = get_fat(cluster_tmp);
    cluster_no++;
  }
  internal->cur_cluster_no = cluster_no;
  internal->cur_cluster = cluster_tmp;
  if (internal->buf == 0){
    internal->buf = MALLOC(char, SECTOR_SIZE);
  }
  get_cluster(internal->cur_cluster, internal->buf);
  return internal->buf[pos_l];
}

int fat32fs_read(struct file* file, void* buf, size_t len){
  if ((file->vnode->mode & F_RD) == 0){
    log_puts((char *) "[Error] Read permission denied\n", WARNING);
    return -2;
  }
  int r = 0;
  struct fat32fs_internal *internal = (struct fat32fs_internal *) file->vnode->internal;
  if (file->f_pos >= internal->size){
    log_puts((char *) "[SEVERE] CPIOFS read file over size!\n", SEVERE);
    return -1;
  }
  for (size_t i = 0; i<len; i++){
    if (file->f_pos < internal->size){
      ((char*)buf)[i] = fat32_get_char(internal, file->f_pos);
      file->f_pos++;
      r++;
    }
    else{
      ((char*)buf)[i] = '\0';
      break;
    }
  }
  return r;
}

static int fat32_put_char(struct fat32fs_internal *internal, size_t pos, char data){
  uint32_t pos_h = pos/SECTOR_SIZE;
  uint32_t pos_l = pos%SECTOR_SIZE;
  if (internal->cur_cluster_no == pos_h){
    if (internal->buf == 0){
      internal->buf = MALLOC(char, SECTOR_SIZE);
      get_cluster(internal->cur_cluster, internal->buf);
    }
    internal->buf[pos_l] = data;
    return 0;
  }
  put_cluster(internal->cur_cluster, internal->buf);
  uint32_t cluster_no = (internal->cur_cluster_no > pos_h) ? 0 : internal->cur_cluster_no;
  uint32_t cluster_tmp = (internal->cur_cluster_no > pos_h) ? internal->first_cluster : internal->cur_cluster;
  while(cluster_no < pos_h){
    cluster_tmp = get_fat(cluster_tmp);
    cluster_no++;
  }
  internal->cur_cluster_no = cluster_no;
  internal->cur_cluster = cluster_tmp;
  if (internal->buf == 0){
    internal->buf = MALLOC(char, SECTOR_SIZE);
  }
  get_cluster(internal->cur_cluster, internal->buf);
  internal->buf[pos_l] = data;
  return 0;
}

int fat32fs_write(struct file* file, const void* buf, size_t len){
  if ((file->vnode->mode & F_WR) == 0){
    log_puts((char *) "[Error] Write permission denied\n", WARNING);
    return -2;
  }
  int r = 0;
  struct fat32fs_internal *internal = (struct fat32fs_internal *) file->vnode->internal;
  uint32_t max_w_size = get_fat_list_argc(internal->first_cluster)*SECTOR_SIZE;
  for (size_t i = 0; i< len; i++){
    if (internal->size < max_w_size){
      fat32_put_char(internal, file->f_pos, ((char *)buf)[i]);
      file->f_pos++;
      r++;
    }
    else{
      break;
    }
  }
  internal->size = file->f_pos;
  if (internal->size < max_w_size){
    fat32_put_char(internal, file->f_pos, '\0');
  }
  put_cluster(internal->cur_cluster, internal->buf);
  return r;
}

int fat32fs_cat(struct vnode *vnode){
  struct fat32fs_internal *internal = (struct fat32fs_internal*)vnode->internal;
  uint32_t next_cluster = internal->first_cluster;
  uint32_t size = internal->size;
  int cluster_p_count = 0;
  while(next_cluster < FAT_END_CLUSTER){
    char buf[SECTOR_SIZE];
    get_cluster(next_cluster, buf);
    next_cluster = get_fat(next_cluster);
    int i;
    for (i=0; i<size; i++){
      if (i == SECTOR_SIZE) break;
      if (buf[i] == '\n') uart_puts("\n");
      else if (buf[i] < 0x20 || buf[i] == 0x7f) uart_write('.');
      else uart_write(buf[i]);
    }
    size -= i;
    if (size == 0){
      return 0;
    }
    cluster_p_count++;
    if (cluster_p_count >= 20) break;
  }
  log_puts("\n\n[INFO] File too large. cat only show first 20 sctors.", INFO);
  return 0;
}

int fat32fs_dir_cat(struct vnode *vnode){
  return -1;
}

int fat32fs_get_size(struct vnode *vnode){
  if (vnode->dentry->type == FILE){
    struct fat32fs_internal *internal = (struct fat32fs_internal*)vnode->internal;
    return internal->size;
  }
  return 0;
}

int fat32fs_rm(struct vnode *vnode){
  log_puts("[Oops] Not implement remove yet.", WARNING);
  return -1;
}

void fat32fs_check_size(struct vnode *vnode){
  struct fat32fs_internal *internal = (struct fat32fs_internal*)vnode->internal;
  struct fat32fs_internal *parent_internal = (struct fat32fs_internal*)vnode->dentry->parent->vnode->internal;
  struct cluster_data *argv = parent_internal->dir_cluster_data;
  
  int idx = internal->dir_t_no;
  if (idx <0 ) return ;
  
  union directory_t *dir_t = get_dir_t(internal->dir_t_no, parent_internal->dir_cluster_data);
  
  if (dir_t->SFN.size != internal->size){
    dir_t->SFN.size = internal->size;
    put_cluster(argv[idx/(SECTOR_SIZE/DIR_S_SIZE)].cluster_num, argv[idx/(SECTOR_SIZE/DIR_S_SIZE)].buf);
    log_puts(" (change file size)\n", INFO);
  }
  else{
    log_puts("\n", INFO);
  }
}

int fat32fs_dir_release(struct vnode *vnode){
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
  struct fat32fs_internal *internal = (struct fat32fs_internal*)vnode->internal;
  if (internal->dir_cluster_data) free(internal->dir_cluster_data);
  free(d);
  free(vnode);
  return 0;
}

int fat32fs_file_release(struct vnode *vnode){
  if (vnode->file){
    vnode->file->vnode = 0;
  }
  struct dentry *d = vnode->dentry;
  list_del(&(d->list));
  log_puts((char *) "[INFO] Release file < ", INFO);
  log_puts(d->name, INFO);
  log_puts((char *) " > .", INFO);
  fat32fs_check_size(vnode);
  struct fat32fs_internal *internal = (struct fat32fs_internal*)vnode->internal;
  if (internal->buf) free(internal->buf);
  free(d);
  free(vnode->internal);
  free(vnode);
  return 0;
}
