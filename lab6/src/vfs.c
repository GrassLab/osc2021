# include "vfs.h"
# include "tmpfs.h"
# include "uart.h"
# include "mem.h"
# include "log.h"
# include "my_string.h"

struct mount rootmount;

void vfs_init(){
  log_puts((char *) "VFS INIT\n", FINE);
  tmpfs_get_mount_fs(&rootmount);
  log_puts((char *) "VFS INIT aa\n", FINE);
  rootmount.fs->setup_mount(rootmount.fs, &rootmount);
  log_puts((char *) "VFS INIT DONE\n", FINE);
}


int register_filesystem(struct filesystem* fs) {
  return fs->setup_mount(fs, &rootmount);
  // register the file system to the kernel.
}

void vfs_uart_puts(char *c, int iter){
  for (int i=0; i<iter; i++){
    uart_puts((char *) "  ");
  }
  uart_puts(c);
}

void vfs_list_dentry(struct dentry *d, int iter){
  struct list_head *head_t;
  list_for_each(head_t, &d->childs){
    struct dentry *dt = container_of(head_t, struct dentry, list);
    vfs_uart_puts(dt->name, iter);
    uart_puts((char *) "\n");
    if (dt->type == DIR){
      vfs_list_dentry(dt, iter+1);
    }
  }
}

void vfs_list_tree(){
  struct dentry *root_dentry = rootmount.root->dentry;
  vfs_list_dentry(root_dentry, 0);
}

void vfs_do_mkdir(char *name){
  struct dentry *root_dentry = rootmount.root->dentry;
  struct vnode *new_v = MALLOC(struct vnode);
  rootmount.root->v_ops->mkdir(rootmount.root, &new_v, name);
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


struct vnode* vfs_create_vnode(){
  struct vnode *new_vnode = MALLOC(struct vnode);
  return new_vnode;
}

struct dentry* vfs_create_dentry(struct dentry* parent, const char* name, enum dentry_type type){
  struct dentry *new_d  = MALLOC(struct dentry);
  list_head_init(&new_d->list);
  list_head_init(&new_d->childs);
  new_d->vnode = vfs_create_vnode();
  new_d->parent = parent;
  str_copy((char *) name, new_d->name);
  new_d->type = DIR;
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
  if (parent){
    list_add_prev(&new_d->list, &parent->childs);
  }
  return new_d;
}

int vfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name){
  if ((dir_node->mode & 4) == 0){
    return -2; //Can't read.
  }
  struct dentry *d = dir_node->dentry;
  struct list_head *head_t;
  list_for_each(head_t, &(d->childs)){
    struct dentry *t = container_of(head_t, struct dentry, list);
    if (str_cmp(t->name, (char *)component_name) == 0){
      return 0;
    }
  }
  return -1;
}
