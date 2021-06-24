#include "vfs.h"

#include "exc.h"
#include "io.h"
#include "sched.h"

dentry root;

static inline dentry *get_pwd() { return (get_taskstruct()->pwd); }

// automatic new_str for two
void split_path_r(const char *path, char **p_path, char **c_path) {
  long len = strlen(path) - 1;
  while (len >= 0) {
    if (path[len] == '/') {
      *c_path = new_str(path + len + 1);
      *p_path = kmalloc(len + 2);
      strcpy_n(*p_path, path, len + 1);
      return;
    }
    len--;
  }
  *c_path = new_str(path);
  *p_path = kmalloc(1);
  **p_path = 0;
}

char *split_path_l(char *s) {
  char *right;
  while (*s) {
    if (*s == '/') {
      right = s + 1;
      *s = 0;
      return right;
    }
    s++;
  }
  return s;
}

int vfs_create(const char *path) {
  char *p_path, *c_path;
  split_path_r(path, &p_path, &c_path);

  dentry *target;
  int stat = vfs_opendent(&target, p_path);
  if (stat >= 0) {
    if (target->d_inode->i_mode != TYPE_DIR) {
      stat = INVALID_PATH;
    } else {
      stat = (target->d_inode->i_op->create == NULL)
                 ? METHOD_NOT_IMP
                 : (*(target->d_inode->i_op->create))(target, c_path);
    }
    vfs_closedent(target);
  }
  kfree(p_path);
  kfree(c_path);
  return stat;
}

int vfs_remove(const char *path) {
  char *p_path, *c_path;
  split_path_r(path, &p_path, &c_path);

  dentry *target;
  int stat = vfs_opendent(&target, p_path);
  if (stat >= 0) {
    if (target->d_inode->i_mode != TYPE_DIR) {
      stat = INVALID_PATH;
    } else {
      cdl_list *c_itr = target->child.bk;
      int is_open = 0;
      while (c_itr != &(target->child)) {
        dentry *c_dent = get_struct_head(dentry, sibli, c_itr);
        if (strcmp(c_dent->name, c_path) == 0) {
          is_open = 1;
          break;
        }
        c_itr = c_itr->bk;
      }
      if (is_open) {
        stat = TARGET_INUSE;
      } else {
        stat = (target->d_inode->i_op->remove == NULL)
                   ? METHOD_NOT_IMP
                   : (*(target->d_inode->i_op->remove))(target, c_path);
      }
    }
    vfs_closedent(target);
  }
  kfree(p_path);
  kfree(c_path);
  return stat;
}

int vfs_mkdir(const char *path) {
  char *p_path, *c_path;
  split_path_r(path, &p_path, &c_path);

  dentry *target;
  int stat = vfs_opendent(&target, p_path);
  if (stat >= 0) {
    if (target->d_inode->i_mode != TYPE_DIR) {
      stat = INVALID_PATH;
    } else {
      stat = (target->d_inode->i_op->mkdir == NULL)
                 ? METHOD_NOT_IMP
                 : (*(target->d_inode->i_op->mkdir))(target, c_path);
    }
    vfs_closedent(target);
  }
  kfree(p_path);
  kfree(c_path);
  return stat;
}

int vfs_rmdir(const char *path) {
  char *p_path, *c_path;
  split_path_r(path, &p_path, &c_path);

  dentry *target;
  int stat = vfs_opendent(&target, p_path);
  if (stat >= 0) {
    if (target->d_inode->i_mode != TYPE_DIR) {
      stat = INVALID_PATH;
    } else {
      cdl_list *c_itr = target->child.bk;
      int is_open = 0;
      while (c_itr != &(target->child)) {
        dentry *c_dent = get_struct_head(dentry, sibli, c_itr);
        if (strcmp(c_dent->name, c_path) == 0) {
          is_open = 1;
          break;
        }
        c_itr = c_itr->bk;
      }
      if (is_open) {
        stat = TARGET_INUSE;
      } else {
        stat = (target->d_inode->i_op->rmdir == NULL)
                   ? METHOD_NOT_IMP
                   : (*(target->d_inode->i_op->rmdir))(target, c_path);
      }
    }
    vfs_closedent(target);
  }
  kfree(p_path);
  kfree(c_path);
  return stat;
}

unsigned long vfs_open(const char *path, unsigned long flag) {
  unsigned long new_fd_num = 0;

  dentry *target;
  int stat = vfs_opendent(&target, path);

  if (stat == TARGET_NO_EXIST && (flag & O_CREATE) != 0) {
    stat = vfs_create(path);
    if (stat >= 0) {
      stat = vfs_opendent(&target, path);
    }
  }

  if (stat >= 0) {
    file *new_file;
    stat = (*(target->d_inode->i_op->open))(target, &new_file, flag);
    vfs_closedent(target);
    if (stat >= 0) {
      task_struct *ts = get_taskstruct();
      file_discriptor *new_fd = kmalloc(sizeof(file_discriptor));
      new_fd->f = new_file;
      new_fd->fd_num = ts->fd_cnt++;
      new_fd_num = new_fd->fd_num;
      push_cdl_list(&(ts->fd_list), &(new_fd->fd_list));
    }
  } else {
    log("open fail", LOG_DEBUG);
  }
  return new_fd_num;
}

file_discriptor *fd_num_to_fd(unsigned long fd) {
  cdl_list *fd_list = &(get_taskstruct()->fd_list);
  cdl_list *fd_itr = fd_list->bk;
  while (fd_itr != fd_list) {
    if (((file_discriptor *)fd_itr)->fd_num == fd) {
      return ((file_discriptor *)fd_itr);
    }
    fd_itr = fd_itr->bk;
  }
  return NULL;
}

int vfs_close(unsigned long fd_num) {
  file_discriptor *fd = fd_num_to_fd(fd_num);
  if (fd == NULL) {
    return INVALID_FD;
  }
  pop_cdl_list(&(fd->fd_list));
  file *f = fd->f;
  kfree(fd);
  return (*(f->f_inode->i_op->close))(f);
}

dentry *dentry_btm_layer(dentry *dent) {
  while (dent->mnt != NULL) {
    dent = dent->mnt->root;
  }
  return dent;
}

dentry *dentry_top_layer(dentry *dent) {
  while (dent == dent->d_inode->i_sb->root &&
         dent->d_inode->i_sb->mnt_p != NULL) {
    dent = dent->d_inode->i_sb->mnt_p;
  }
  return dent;
}

int vfs_opendent(struct dentry **target, const char *path) {
  // log("od ", LOG_PRINT);
  // log(path, LOG_PRINT);
  // log("\n", LOG_PRINT);
  dentry *t_itr;
  unsigned long offset = 0;

  if (*path == '/') {
    t_itr = &root;
    offset = 1;
  } else {
    t_itr = get_pwd();
    if (*path == '.') {
      if (*(path + 1) == '/') {
        offset = 2;
      } else if (*(path + 1) == 0) {
        offset = 1;
      }
    }
  }
  disable_interrupt();
  t_itr = dentry_btm_layer(t_itr);
  t_itr->ref_cnt++;
  enable_interrupt();
  char *path_copy = new_str(path + offset);
  char *path_itr = path_copy;
  while (*path_itr != 0) {
    char *next_path = split_path_l(path_itr);
    if (*path_itr != 0 && strcmp(path_itr, ".") != 0) {
      if (strcmp(path_itr, "..") == 0) {
        disable_interrupt();
        dentry *top_l = dentry_top_layer(t_itr);
        if (top_l->parent != NULL) {
          top_l = top_l->parent;
        }
        top_l = dentry_btm_layer(top_l);
        top_l->ref_cnt++;
        enable_interrupt();
        vfs_closedent(t_itr);
        t_itr = top_l;
      } else {
        disable_interrupt();
        cdl_list *head = &(t_itr->child);
        cdl_list *c_itr = head->bk;
        while (c_itr != head) {
          dentry *child_dent = get_struct_head(dentry, sibli, c_itr);
          if (strcmp(path_itr, child_dent->name) == 0) {
            t_itr->ref_cnt--;
            // child_dent->ref_cnt++;
            t_itr = dentry_btm_layer(child_dent);
            t_itr->ref_cnt++;
            break;
          }
          c_itr = c_itr->bk;
        }
        enable_interrupt();
        if (c_itr == head) {
          dentry *child_dent;
          int stat =
              (*(t_itr->d_inode->i_op->opendent))(t_itr, &child_dent, path_itr);
          if (stat < 0) {
            vfs_closedent(t_itr);
            return stat;
          }
          disable_interrupt();
          t_itr->ref_cnt--;
          enable_interrupt();
          t_itr = child_dent;
        }
      }
    }
    path_itr = next_path;
  }
  kfree(path_copy);
  *target = t_itr;
  return 0;
}

int vfs_closedent(struct dentry *target) {
  int stat = 0;
  disable_interrupt();
  target->ref_cnt--;
  if (target->ref_cnt == 0) {
    stat = (*(target->d_inode->i_op->closedent))(target);
  }
  enable_interrupt();
  return stat;
}

int vfs_getdent(unsigned long fd_num, unsigned long count,
                struct dirent *dent) {
  file_discriptor *fd = fd_num_to_fd(fd_num);
  if (fd == NULL) {
    return INVALID_FD;
  };
  struct dentry *target = fd->f->path;
  if (target->d_inode->i_mode != TYPE_DIR) {
    return INVALID_PATH;
  }

  return (target->d_inode->i_op->getdent == NULL)
             ? METHOD_NOT_IMP
             : (*(target->d_inode->i_op->getdent))(target, count, dent);
}

long vfs_read(unsigned long fd_num, char *buf, unsigned long size) {
  file_discriptor *fd = fd_num_to_fd(fd_num);
  if (fd == NULL) {
    return INVALID_FD;
  }
  file *f = fd->f;
  return (*(f->f_op->read))(f, buf, size);
}

long vfs_write(unsigned long fd_num, const char *buf, unsigned long size) {
  file_discriptor *fd = fd_num_to_fd(fd_num);
  if (fd == NULL) {
    return INVALID_FD;
  }
  file *f = fd->f;
  return (*(f->f_op->write))(f, buf, size);
}

int not_imp() { return METHOD_NOT_IMP; }

cdl_list fs_list;
file_system_t root_fs;
super_block root_sb;
inode root_i;
inode_op root_i_op;
dentry_op root_d_op;

void init_vfs() {
  disable_interrupt();
  root.name = "";
  root.d_op = &root_d_op;
  root_d_op.umount = &not_imp;
  root.parent = NULL;
  init_cdl_list(&(root.child));
  init_cdl_list(&(root.sibli));
  root.mnt = NULL;
  root.d_data = NULL;
  root.ref_cnt = 1;
  root.d_inode = &root_i;
  root_i.i_sb = &root_sb;
  root_i.i_op = &root_i_op;
  root_i.i_mode = TYPE_DIR;
  root_i.i_size = 0;
  root_i.i_data = NULL;
  memset_ul(&root_i_op, (unsigned long)(&not_imp), sizeof(inode_op));
  init_cdl_list(&(fs_list));
  init_cdl_list(&(root_fs.sb_list));
  push_cdl_list(&(fs_list), &(root_fs.fs_list));
  root_fs.name = "";
  root_fs.mount = &not_imp;
  push_cdl_list(&(root_fs.sb_list), &(root_sb.sb_list));
  root_sb.root = &root;
  root_sb.mnt_p = NULL;
  enable_interrupt();
}

int vfs_mount(const char *dev, const char *mp, const char *fs) {
  cdl_list *fs_itr = fs_list.bk;
  file_system_t *mfs = NULL;
  while (fs_itr != &(fs_list)) {
    if (strcmp(((file_system_t *)fs_itr)->name, fs) == 0) {
      mfs = (file_system_t *)fs_itr;
      break;
    }
    fs_itr = fs_itr->bk;
  }
  if (mfs == NULL) {
    return INVALID_FS;
  }
  dentry *mdent;
  int stat = vfs_opendent(&mdent, mp);
  if (stat < 0) {
    return stat;
  }
  disable_interrupt();
  mdent = dentry_btm_layer(mdent);
  stat = (*(mfs->mount))(mfs, mdent, dev);
  enable_interrupt();
  vfs_closedent(mdent);
  mdent = dentry_btm_layer(mdent);
  return stat;
}

int vfs_chdir(const char *path) {
  dentry *new_pwd;
  int stat = vfs_opendent(&new_pwd, path);
  if (stat < 0) {
    return stat;
  }
  if (new_pwd->d_inode->i_mode != TYPE_DIR) {
    vfs_closedent(new_pwd);
    return INVALID_PATH;
  }

  task_struct *ts = get_taskstruct();
  vfs_closedent(ts->pwd);
  ts->pwd = new_pwd;
  return 0;
}

dentry *get_vfs_root() {
  disable_interrupt();
  root.ref_cnt++;
  enable_interrupt();
  return &root;
}

void register_fs(file_system_t *fs) {
  disable_interrupt();
  push_cdl_list(&(fs_list), &(fs->fs_list));
  enable_interrupt();
}

int vfs_umount(const char *path) {
  dentry *mdent;
  int stat = vfs_opendent(&mdent, path);
  if (stat < 0) {
    return stat;
  }
  disable_interrupt();
  mdent = dentry_btm_layer(mdent);
  if (mdent == mdent->d_inode->i_sb->root) {
    mdent->ref_cnt--;
    stat = (*(mdent->d_op->umount))(mdent);
    enable_interrupt();
  } else {
    stat = INVALID_PATH;
    enable_interrupt();
    vfs_closedent(mdent);
  }
  return stat;
}

unsigned long get_filesize(unsigned long fd_num) {
  file_discriptor *fd = fd_num_to_fd(fd_num);
  if (fd == NULL) {
    return 0;
  }
  return fd->f->f_inode->i_size;
}