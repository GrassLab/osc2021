# include "vfs.h"
# include "sd.h"
# include "typedef.h"

# define FAT32FS_DEFAULT_MODE   6
# define FAT32FS_CAT_LB         0x20

struct fat32fs_internal{
  uint32_t size;
  uint32_t first_cluster;
  uint32_t cur_cluster;
  int cur_cluster_no;
  struct cluster_data *dir_cluster_data;
  int dir_t_no;
  char *buf;
};

//void tmpfs_get_mount_fs(struct mount* mount_point);
struct filesystem* fat32fs_get_fs();
int fat32fs_setup_mount(struct filesystem* fs, struct mount* mount);
int fat32fs_unmount(struct mount *mount);
int fat32fs_mkdir(struct vnode* dir_name, struct vnode** target, const char* component_name);
int fat32fs_create(struct vnode* dir_name, struct vnode** target, const char* component_name);
int fat32fs_read(struct file* file, void* buf, size_t len);
int fat32fs_write(struct file* file, const void* buf, size_t len);
int fat32fs_cat(struct vnode *vnode);
//int fat32fs_dir_cat(struct vnode *vnode);
int fat32fs_get_size(struct vnode *vnode);
int fat32fs_rm(struct vnode *vnode);
int fat32fs_dir_release(struct vnode *vnode);
int fat32fs_file_release(struct vnode *vnode);
