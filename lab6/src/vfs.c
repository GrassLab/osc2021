# include "vfs.h"
# include "mem.h"
# include "my_string.h"

struct mount* rootmount;

void vfs_init(){
}

struct vnode* vfs_create_vnode(){
  struct vnode *new_vnode  = MALLOC(struct vnode);
  return new_vnode;
}

struct dentry* vfs_create_dentry(struct dentry* parent, char* name, enum dentry_type type){
  struct dentry *new_d  = MALLOC(struct dentry);
  list_head_init(&new_d->list);
  list_head_init(&new_d->childs);
  new_d->vnode = vfs_create_vnode();
  new_d->parent = parent;
  str_copy(name, new_d->name);
  if (type == DIR){
    struct dentry *new_d1 = MALLOC(struct dentry);
    struct dentry *new_d2 = MALLOC(struct dentry);
    list_head_init(&new_d1->list);
    list_head_init(&new_d1->childs);
    list_head_init(&new_d2->list);
    list_head_init(&new_d2->childs);
    new_d1->vnode = new_d->vnode;
    new_d2->vnode = (parent) ? parent->vnode : new_d->vnode;
    new_d1->parent = 0;
    new_d2->parent = 0;
    new_d1->name[0] = '.';  new_d1->name[1] = '\0';
    new_d2->name[0] = '.';  new_d2->name[1] = '.';  new_d2->name[2] = '\0';
    new_d1->type = SDIR;
    new_d2->type = SDIR;
    list_add_prev(&new_d1->list, &new_d->childs);
    list_add_prev(&new_d2->list, &new_d->childs);
  }
  return new_d;
}

int register_filesystem(struct filesystem* fs) {
  return fs->setup_mount(fs, &rootmount);
  // register the file system to the kernel.
}

struct file* vfs_open(const char* pathname, int flags) {
  return 0;
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
}
int vfs_close(struct file* file) {
  return 0;
  // 1. release the file descriptor
}
int vfs_write(struct file* file, const void* buf, size_t len) {
  return 0;
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
}
int vfs_read(struct file* file, void* buf, size_t len) {
  return 0;
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
}
