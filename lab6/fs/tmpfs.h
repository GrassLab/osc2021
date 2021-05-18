#ifndef _TMPFS_H_
#define _TMPFS_H_
#include <types.h>
#include <vfs.h>

enum tmpfs_type
{
  dir_t,
  file_t
};

struct tmpfs_inode {
  enum tmpfs_type type;   
  char name[FILE_NAME_LEN];
  struct tmpfs_block *block;
  struct tmpfs_inode* sublings;
  struct tmpfs_inode* children;
  size_t size;
  struct vnode *vnode; 
};

struct tmpfs_block {
  size_t size;
  struct tmpfs_block *next;
  char content[FS_BLOCK_SIZE];
};

struct file_operations tmpfs_fops;

struct vnode_operations tmpfs_vops;

void tmpfs_init();
void* tmpfs_vnode_create(struct mount* _mount, enum tmpfs_type type);
static int setup_mount(struct filesystem* fs, struct mount* _mount);
static int write(struct file* file, const void* buf, size_t len);
static int read(struct file* file, void* buf, size_t len);
static int lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
static int create(struct vnode* dir_node, struct vnode** target, const char* component_name);
void traversal(struct tmpfs_inode* inode);

int tmpfs_load_initramfs(struct mount* _mount);
int tmpfs_copy_content_from_cpio(struct tmpfs_inode* inode, void* addr, size_t size);
#endif