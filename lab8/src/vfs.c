# include "vfs.h"
# include "tmpfs.h"
# include "uart.h"
# include "mem.h"
# include "log.h"
# include "my_string.h"
# include "my_math.h"
# include "schedule.h"
# include "exception.h"
# include "flags.h"
# include "cpiofs.h"
# include "fat32fs.h"

struct mount rootmount;

void vfs_init(){
  log_puts("[INFO] VFS Init.\n", INFO);
  struct dentry *new_d = vfs_create_dentry(0, (char *)"/", DIR);
  log_puts("[INFO] Mount tmpfs on / \n", INFO);
  struct filesystem *tmpfs= tmpfs_get_fs();
  register_filesystem(&rootmount, tmpfs, new_d->vnode);
  
  log_puts("[INFO] tmpfs mount DONE\n", INFO);
  log_puts("[INFO] VFS Init DONE\n", INFO);
}


int register_filesystem(struct mount *mount, struct filesystem* fs, struct vnode *vnode) {
  mount->fs = fs;
  mount->root = vnode;
  if (vnode->dentry->type != DIR){
    log_puts("[Error] Mount point should be dir.\n", WARNING);
    return -1;
  }
  if (vnode->v_ops){
    if(vnode->v_ops->size(vnode) != 2){
      log_puts("[Error] Mount point is not empty.\n", WARNING);
      return -1;
    }
  }
  return fs->setup_mount(fs, mount);
  // register the file system to the kernel.
}

void vfs_uart_puts(char *c, int iter){
  for (int i=0; i<iter; i++){
    uart_puts("  ");
  }
  uart_puts(c);
}

void vfs_list_dentry(struct dentry *d, int iter, int rev){
  struct list_head *head_t;
  list_for_each(head_t, &d->childs){
    struct dentry *dt = container_of(head_t, struct dentry, list);
    if (dt->type == DIR) uart_puts(" d");
    else if (dt->type == SDIR) uart_puts(" d");
    else if (dt->type == FILE) uart_puts(" -");
    if (dt->vnode->mode & F_RD) uart_puts((char *)"r");
    else uart_puts((char *)"-");
    if (dt->vnode->mode & F_WR) uart_puts((char *)"w");
    else uart_puts((char *)"-");
    if (dt->vnode->mode & F_EX) uart_puts((char *)"x");
    else uart_puts((char *)"-");
    uart_puts((char *)"\t");
    int size = dt->vnode->v_ops->size(dt->vnode);
    char ct[20];
    int_to_str(size, ct);
    uart_puts(ct);
    if (dt->type == DIR) uart_puts("  items");
    else if (dt->type == SDIR) uart_puts("  items");
    else if (dt->type == FILE) uart_puts("  bytes");
    uart_puts((char *)"\t");
    vfs_uart_puts(dt->name, iter);
    uart_puts("\n");
    if (rev == 1 && dt->type == DIR){
      vfs_list_dentry(dt, iter+1, 1);
    }
  }
}

void vfs_list_tree(){
  struct dentry *root_dentry = rootmount.root->dentry;
  vfs_list_dentry(root_dentry, 0, 1);
}

int do_mkdir(char *name, struct vnode *dir_node){
  struct vnode *new_v = MALLOC(struct vnode, 1);
  struct vnode *new_vt = new_v;
  int r = dir_node->v_ops->lookup(dir_node, &new_vt, name);
  if (r == -1){
    dir_node->v_ops->mkdir(dir_node, &new_v, name);
    return 0;
  }
  else{
    free(new_v);
    return -1;
  }
}



struct vnode* vfs_create_vnode(){
  struct vnode *new_vnode = MALLOC(struct vnode, 1);
  new_vnode->mount = 0;
  new_vnode->mode = 0;
  new_vnode->v_ops = 0;
  new_vnode->f_ops = 0;
  new_vnode->internal = 0;
  new_vnode->file = 0;
  new_vnode->dentry = 0;
  return new_vnode;
}

struct dentry* vfs_create_dentry(struct dentry* parent, const char* name, enum dentry_type type){
  struct dentry *new_d  = MALLOC(struct dentry, 1);
  list_head_init(&new_d->list);
  list_head_init(&new_d->childs);
  new_d->vnode = vfs_create_vnode();
  new_d->vnode->dentry = new_d;
  new_d->parent = parent;
  str_copy(name, new_d->name);
  new_d->type = type;
  if (type == DIR){
    struct dentry *new_d1 = MALLOC(struct dentry, 1);
    struct dentry *new_d2 = MALLOC(struct dentry, 1);
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
  if ((dir_node->mode & F_RD) == 0){
    return -2; //Can't read.
  }
  struct dentry *d = dir_node->dentry;
  struct list_head *head_t;
  list_for_each(head_t, &(d->childs)){
    struct dentry *t = container_of(head_t, struct dentry, list);
    if (str_cmp(t->name, (char *)component_name) == 1){
      *target = t->vnode;
      return 0;
    }
  }
  return -1;
}

struct vnode* get_root_vnode(){
  return rootmount.root;
}

int get_pwd_string(struct vnode *v, char *s){
  if (v->dentry->parent){
    int r = get_pwd_string(v->dentry->parent->vnode, s);
    if (r > 0 && s[r-1] == '/'){
      str_copy(v->dentry->name, (s+r));
      int name_len = str_len(v->dentry->name);
      return r+name_len;
    }
    else{
      s[r] = '/';
      str_copy(v->dentry->name, (s+r+1));
      int name_len = str_len(v->dentry->name);
      return r+name_len+1;
    }
  }
  else{
    s[0] = '/';
    s[1] = '\0';
    return 1;
  }
}

int vfs_split_path(char *path, char ***list){
  int rv = -1;
  if (path[0] == '/') path++;
  while(*path){
    rv++;
    if (path[0] == '/') return -1;
    (*list)[rv] = path;
    char *t = path;
    while(*t && (*t != '/')){
      t++;
    }
    if (*t == '\0'){
      break;
    }
    *t = '\0';
    path = t+1;
  }
  return rv+1;
}

int get_vnode_by_path(struct vnode *dir_node, struct vnode **target, const char *path){
  char *new_path = MALLOC(char, str_len(path)+1);
  char **vnode_argv = MALLOC(char*, 20);
  str_copy(path, new_path);
  int vnode_argc = vfs_split_path(new_path, &vnode_argv);
  struct vnode *target_t = 0;
  if (path[0] == '/'){
    dir_node = get_root_vnode();
    target_t = dir_node;
  }
  for (int i = 0; i<vnode_argc; i++){
    log_puts("Get vnode by path, get ", FINE);
    log_puts(vnode_argv[i], FINE);
    log_puts("\n", FINE);
    int rt = dir_node->v_ops->lookup(dir_node, &target_t, vnode_argv[i]);
    if (rt != 0){
      free(new_path);
      free(vnode_argv);
      return -1;
    }
    dir_node = target_t;
  }
  *target = target_t;
  free(new_path);
  free(vnode_argv);
  return 0;
}


int do_cd(char *path){
  struct task *cur = get_current();
  struct vnode *cur_vnode = cur->pwd_vnode;
  struct vnode *target = 0;
  int r = get_vnode_by_path(cur_vnode, &target, path);
  if (r == 0){
    cur->pwd_vnode = target;
  }
  return r;
}

void do_ls(char *path){
  struct task *cur = get_current();
  struct vnode *ls_vnode = cur->pwd_vnode;
  struct vnode *target = 0;
  if (path){
    int r = get_vnode_by_path(ls_vnode, &target, path);
    if (r != 0){
      uart_puts("Path ");
      uart_puts(path);
      uart_puts(" not found.\n");
      return ;
    }
    ls_vnode = target;
  }
  if (ls_vnode->dentry->type != DIR){
    log_puts("[Error] < ", WARNING);
    log_puts(path, WARNING);
    log_puts(" > is not a directory.\n", WARNING);
    return ;
  }
  vfs_list_dentry(ls_vnode->dentry, 0, 0);
}

void do_cat(char *path){
  struct task *cur = get_current();
  struct vnode *cat_vnode = cur->pwd_vnode;
  struct vnode *target = 0;
  if (path){
    int r = get_vnode_by_path(cat_vnode, &target, path);
    if (r != 0){
      uart_puts("Path < ");
      uart_puts(path);
      uart_puts(" > not found.\n");
      return ;
    }
    cat_vnode = target;
  }
  int cr = cat_vnode->v_ops->cat(cat_vnode);
  if (cr == -1){
    uart_puts("Can't  cat file < ");
    uart_puts(path);
    uart_puts(" > .\n");
  }
}

struct file* vfs_open(const char* pathname, int flags, int *errno) {
  int pathname_len = str_len(pathname);
  if (pathname_len == 0){
    *errno = -1;
    return 0;
  }
  char dirname[pathname_len+1];
  str_copy(pathname, dirname);
  // Split Dir name and File name
  char *filename = 0;
  for (int i = pathname_len-1; i >= 0; i--){
    if (dirname[i] == '/'){
      dirname[i] = '\0';
      filename = &dirname[i+1];
      break;
    }
  }
  struct task *cur = get_current();
  struct vnode *dvnode = cur->pwd_vnode;
  int dir_exist = 0;
  if (filename == 0){
    filename = dirname;
  }
  else if(dirname[0] == '\0'){
    dvnode = get_root_vnode();
  }
  else{
    struct vnode *tvnode = 0;
    dir_exist = get_vnode_by_path(dvnode, &tvnode, dirname);
    dvnode = tvnode;
  }
  // Check dir exist
  if (dir_exist != 0){
    *errno = -1;
    return 0;
  }
  // Get dir vnode
  struct vnode *target;
  int lpr = dvnode->v_ops->lookup(dvnode, &target, filename);

  // Create file
  if (lpr){
    if ((flags & O_CREAT) == 0){
      log_puts("[Error] No Create flag\n", WARNING);
      *errno = -1;
      return 0;
    }
    int tr = dvnode->v_ops->create(dvnode, &target, filename);
    if (tr){
      *errno = -1;
      return 0;
    }
  }

  if (flags & O_RD){
    if ((target->mode & F_RD) == 0){
      *errno = -2;
      return 0;
    }
  }
  if (flags & O_WR){
    if ((target->mode & F_WR) == 0){
      *errno = -2;
      return 0;
    }
  }
  struct file *new_file = MALLOC(struct file, 1);
  new_file->vnode = target;
  new_file->f_pos = 0;
  new_file->flag = flags;
  return new_file;
}

int vfs_close(struct file* file) {
  file->vnode->file = 0;
  free(file);
  return 0;
}

int vfs_get_dir_size(struct vnode *vnode){
  struct dentry *dentry = vnode->dentry;
  struct list_head *head = &(dentry->childs);
  struct list_head *pos;
  int r = 0;
  list_for_each(pos, head){
    r++;
  }
  return r;
}

int do_open(const char *pathname, int flags){
  int errno = 0;
  struct file *new_file = vfs_open(pathname, flags, &errno);
  if (new_file == 0){
    return errno;
  }
  int fd = get_new_fd(new_file);
  return fd;
}

int do_close(int fd){
  struct file *file = get_file_by_fd(fd);
  return vfs_close(file);
}

int do_write(int fd, const void* buf, size_t len) {
  struct file *file = get_file_by_fd(fd);
  if (file == 0){
    return -1;
  }
  if ((file->flag & O_WR) == 0){
    return -2;
  }
  return file->vnode->f_ops->write(file, buf, len);
}

int do_read(int fd, void* buf, size_t len) {
  struct file *file = get_file_by_fd(fd);
  if (file == 0){
    return -1;
  }
  if ((file->flag & O_RD) == 0){
    return -2;
  }
  return file->vnode->f_ops->read(file, buf, len);
}

void do_rm(char *path){
  struct task *cur = get_current();
  struct vnode *rm_vnode = cur->pwd_vnode;
  struct vnode *target = 0;
  if (path){
    int r = get_vnode_by_path(rm_vnode, &target, path);
    if (r != 0){
      uart_puts("Path < ");
      uart_puts(path);
      uart_puts(" > not found.\n");
      return ;
    }
    rm_vnode = target;
  }
  int cr = rm_vnode->v_ops->rm(rm_vnode);
  if (cr == -1){
    uart_puts("Can't rm file < ");
    uart_puts(path);
    uart_puts(" > .\n");
  }
}

int do_mount(const char *mountpoint, const char *fsname){
  struct filesystem *fs = 0;
  if (str_cmp(fsname, (char *)"tmpfs") == 1){
    fs = tmpfs_get_fs();
  }
  else if (str_cmp(fsname, (char *)"cpiofs") == 1){
    fs = cpiofs_get_fs();
  }
  else if (str_cmp(fsname, (char *)"fat32fs") == 1){
    fs = fat32fs_get_fs();
  }
  if (fs){
    struct task *cur = get_current();
    struct vnode *target = 0;
    int r = get_vnode_by_path(cur->pwd_vnode, &target, mountpoint);
    if (r){
      log_puts((char*) "[Error] Mount point not found\n", WARNING);
      return -1;
    }
    struct mount *mount = MALLOC(struct mount, 1);
    return register_filesystem(mount, fs, target);
  }
  return -1;
}

int do_unmount(char *path){
  struct task *cur = get_current();
  struct vnode *um_vnode = cur->pwd_vnode;
  struct vnode *target = 0;
  if (path){
    int r = get_vnode_by_path(um_vnode, &target, path);
    if (r != 0){
      uart_puts("Path < ");
      uart_puts(path);
      uart_puts(" > not found.\n");
      return -1;
    }
    um_vnode = target;
  }
  if (um_vnode != um_vnode->mount->root){
    uart_puts("Path < ");
    uart_puts(path);
    uart_puts(" > is not the mountpoint.\n");
    return -1;
  }
  if (um_vnode != um_vnode->mount->root){
  }
  int cr = um_vnode->mount->fs->unmount(um_vnode->mount);
  if (cr == -1){
    uart_puts("Unmout fs ERROR \n");
  }
  if (um_vnode->dentry->parent){
    um_vnode->mount = um_vnode->dentry->parent->vnode->mount;
    um_vnode->v_ops = um_vnode->dentry->parent->vnode->v_ops;
    um_vnode->f_ops = um_vnode->dentry->parent->vnode->f_ops;
  }
  return 0;
}

void sys_open(struct trapframe *arg){
  const char *pathname = (const char *) arg->x[0];
  int flags = (int) arg->x[1];
  int r = do_open(pathname, flags);
  arg->x[0] = (uint64_t)r;
}

void sys_close(struct trapframe *arg){
  int fd = (int) arg->x[0];
  int r = do_close(fd);
  struct task *cur = get_current();
  cur->fd[fd] = 0;
  arg->x[0] = (uint64_t)r;
}

void sys_write(struct trapframe *arg){
  int fd = (int) arg->x[0];
  const char *buf = (const char *) arg->x[1];
  int count = (int) arg->x[2];
  int r = do_write(fd, buf, count);
  arg->x[0] = (uint64_t)r;
}

void sys_read(struct trapframe *arg){
  int fd = (int) arg->x[0];
  char *buf = (char*) arg->x[1];
  int count = (int) arg->x[2];
  int r = do_read(fd, buf, count);
  arg->x[0] = (uint64_t)r;
}

void sys_mkdir(struct trapframe *arg){
  char *name = (char *)arg->x[0];
  struct task *cur = get_current();
  int r = do_mkdir(name, cur->pwd_vnode);
  arg->x[0] = (unsigned long long)r;
}

void sys_chdir(struct trapframe *arg){
  char *name = (char *)arg->x[0];
  int r = do_cd(name);
  arg->x[0] = (unsigned long long)r;
}

void sys_mount(struct trapframe *arg){
  char *mountpoint = (char*) arg->x[1];
  char *fsname = (char*) arg->x[2];
  int r = do_mount(mountpoint, fsname);
  arg->x[0] = (unsigned long long)r;
}

void sys_unmount(struct trapframe *arg){
  char *path = (char*) arg->x[0];
  int r = do_unmount(path);
  arg->x[0] = (unsigned long long)r;
}
