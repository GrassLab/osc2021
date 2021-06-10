#include <stddef.h>
#include <fs/vfs.h>
#include <printf.h>
#include <string.h>
#include <mm.h>
#include <fs/tmpfs.h>
#include <list.h>
#include <current.h>
#include <stat.h>
#include <file.h>
#include <interrupt.h>
#include <fs/open.h>
#include <preempt.h>
#include <syscall_wrapper.h>
#include <asm/errno.h>

struct filesystem *filesystem_list;
struct mount *rootfs;

struct file_iter {
  struct list_head *org;
  struct list_head *now;
};

struct file_stat {
  char *name;
  size_t size;
  unsigned int f_mode;
};

long dir_write(struct file *file, const void *buf, size_t len) {
  if (file->f_pos) {
    kfree((void *)file->f_pos);
  }

  return 0;
}

long dir_read(struct file *file, void *buf, size_t len) {
  struct file_iter *it;
  struct file_stat *stat = buf;
  int ret = 0;

  if (!file->f_pos) {
    it = kmalloc(sizeof(struct file_iter));
    it->org = &file->vnode->subnodes;
    it->now = it->org->next;

    file->f_pos = (size_t)it;
  }

  it = (struct file_iter *)file->f_pos;

  if (it->now != it->org) {
    struct vnode *node = list_entry(it->now, struct vnode, nodes);
    size_t slen = strlen(node->name);
    len -= 1;

    if (slen > len) {
      ret = -ERANGE;
    } else {
      len = slen;
      ret = len;
    }

    memcpy(stat->name, node->name, len);
    stat->name[len] = '\0';
    stat->size = node->size;
    stat->f_mode = node->f_mode;

    it->now = node->nodes.next;
  }

  return ret;
}

size_t dir_lseek(struct file *file, size_t offset, int whence) {
  struct file_iter *it = kmalloc(sizeof(struct file_iter));
  it->org = &file->vnode->subnodes;
  it->now = it->org->next;

  file->f_pos = (size_t)it;

  return 0;
}

struct file_operations dir_ops = {
    .read = &dir_read,
    .write = &dir_write
};

struct filesystem **find_filesystem(const char *name) {
  struct filesystem **p;
  int len = strlen(name);

	for (p = &filesystem_list; *p; p = &(*p)->next) {
		if (strncmp((*p)->name, name, len) == 0 && !(*p)->name[len])
			break;
  }

  return p;
}

int register_filesystem(struct filesystem* fs) {
  size_t flags = disable_irq_save();
  int ret = 0;

  struct filesystem **p = find_filesystem(fs->name);

  /* name has been registered */
  if (*p) {
    ret = -1;
  }

  *p = fs;

  irq_restore(flags);

	return ret;
}

static struct vnode *get_parent_vnode(struct vnode *node) {
  if (node->mnt->root == node) {
    return node->mnt->mountpoint->parent;
  }

  return node->parent;
}

/* TODO: strip path */
/* TODO: add real vnode to represent . and .. */
/* find the vnode of path or the vnode of its parent */
static struct vnode *traverse_path(const char *pathname, int last_dir) {
  struct vnode *root, *now, *target;
  char *p, *pp, *name;

  root = current->fs.root.node;

  if (!last_dir && !strcmp(pathname, "/")) {
    return root;
  }

  p = pp = strdup(pathname);

  if (p[0] == '/') {
    now = root;
    p++;
  } else {
    now = current->fs.pwd.node;
  }

  target = now;

  while (p) {
    name = strsep(&p, "/");

    if (last_dir && !p) {
      if (!strcmp(name, ".") || !strcmp(name, "..")) {
        target = NULL;
      }
      break;
    }

    if (!strcmp(name, ".")) {
      continue;
    }

    if (!strcmp(name, "..")) {
      /* ensure no escape from root */
      if (now != root) {
        now = get_parent_vnode(now);
        target = now;
      }
      continue;
    }

    int ret = now->v_ops->lookup(now, &target, name);
    if (ret || !target) {
      target = NULL;
      break;
    }

    /* if the path escapes mountpoint, go into the mounted root */
    if (target->mnt != now->mnt) {
      target = target->mnt->root;
    }

    now = target;
  }

  kfree(pp);

  return target;
}

// char *normalize_path(const char *path) {

// }

struct file* vfs_open(const char *pathname, int flags) {
  struct file *f;

  if (flags & O_CREAT) {
    struct vnode *dir = traverse_path(pathname, 1);
    if (!dir) {
      return NULL;
    }

    if (!S_ISDIR(dir->f_mode)) {
      return NULL;
    }

    /* TODO: fuck those crabs */
    const char *name;
    name = pathname + strlen(pathname);
    while (*name != '/' && name > pathname) name--;
    if (*name == '/') name++;

    struct vnode *target;
    int ret;

    ret = dir->v_ops->lookup(dir, &target, name);
    if (ret || target) {
      return NULL;
    }

    ret = dir->v_ops->create(dir, name);
    if (ret) {
      return NULL;
    }

    ret = dir->v_ops->lookup(dir, &target, name);
    if (ret || !target) {
      return NULL;
    }

    f = kmalloc(sizeof(struct file));
    f->vnode = target;
    f->f_ops = target->f_ops;
    f->f_mode = target->f_mode;
    f->refcnt = 1;
    f->f_pos = 0;

  } else if (flags & O_DIRECTORY) {
    struct vnode *target = traverse_path(pathname, 0);
    if (!target) {
      return NULL;
    }

    if (!S_ISDIR(target->f_mode)) {
      return NULL;
    }

    f = kmalloc(sizeof(struct file));
    f->vnode = target;
    f->f_ops = &dir_ops;
    f->f_mode = target->f_mode;
    f->refcnt = 1;
    f->f_pos = 0;

  } else {
    struct vnode *target = traverse_path(pathname, 0);
    if (!target) {
      return NULL;
    }

    if (!S_ISREG(target->f_mode)) {
      return NULL;
    }

    f = kmalloc(sizeof(struct file));
    f->vnode = target;
    f->f_ops = target->f_ops;
    f->f_mode = target->f_mode;
    f->refcnt = 1;
    f->f_pos = 0;
  }

  return f;
}

ssize_t vfs_read(struct file *file, void *buf, size_t len) {
  return file->f_ops->read(file, buf, len);
}

ssize_t vfs_write(struct file *file, const void *buf, size_t len) {
  return file->f_ops->write(file, buf, len);
}

int vfs_close(struct file *file) {
  /* sync contents to hardware storage */
  if (S_ISREG(file->vnode->f_mode)) {
    file->vnode->f_ops->fsync(file);
  }

  disable_preempt();
  file->refcnt -= 1;
  enable_preempt();

  if (file->refcnt == 0) {
    kfree(file);
  }

  return 0;
}

int vfs_fsync(struct file *file) {
  if (S_ISREG(file->vnode->f_mode)) {
    return file->vnode->f_ops->fsync(file);
  }

  return -EINVAL;
}

struct vnode *vfs_lookup(struct vnode* dir_node, const char *component_name) {
  struct vnode *target;
  int ret = dir_node->v_ops->lookup(dir_node, &target, component_name);
  if (ret) {
    panic("lookup failed: %s", component_name);
  }

  return target;
}

int vfs_mkdir(struct vnode* dir_node, const char *component_name) {
  return dir_node->v_ops->mkdir(dir_node, component_name);
}

int vfs_rmdir(struct vnode* dir_node, const char *component_name) {
  return dir_node->v_ops->rmdir(dir_node, component_name);
}

int vfs_unlink(struct vnode* dir_node, const char *component_name) {
  return dir_node->v_ops->unlink(dir_node, component_name);
}

SYSCALL_DEFINE1(mkdir, const char *, path) {
  struct vnode *dir = traverse_path(path, 1);
  if (!dir) {
    return -EINVAL;
  }

  if (!S_ISDIR(dir->f_mode)) {
    return -ENOTDIR;
  }

  /* TODO: fuck those crabs */
  const char *name;
  name = path + strlen(path);
  while (*name != '/' && name > path) name--;
  if (*name == '/') name++;

  struct vnode *target;
  int ret;

  ret = dir->v_ops->lookup(dir, &target, name);
  if (ret || target) {
    return ret;
  }

  ret = dir->v_ops->mkdir(dir, name);
  if (ret) {
    return ret;
  }

  return 0;
}

SYSCALL_DEFINE1(rmdir, const char *, path) {
  struct vnode *dir = traverse_path(path, 0);
  if (!dir) {
    return -EINVAL;
  }

  if (!S_ISDIR(dir->f_mode)) {
    return -ENOTDIR;
  }

  /* TODO: fuck those crabs */
  const char *name;
  name = path + strlen(path);
  while (*name != '/' && name > path) name--;
  if (*name == '/') name++;

  if (!list_empty(&dir->subnodes)) {
    return -ENOTEMPTY;
  }

  int ret = dir->v_ops->rmdir(dir, name);
  return ret;
}

SYSCALL_DEFINE1(chdir, const char *, path) {
  struct vnode *dir = traverse_path(path, 0);
  if (!dir) {
    return -ENOENT;
  }

  if (!S_ISDIR(dir->f_mode)) {
    return -ENOTDIR;
  }

  current->fs.pwd.mnt = dir->mnt;
  current->fs.pwd.node = dir;

  return 0;
}

struct vnode_path {
  struct vnode *node;
  struct vnode_path *next;
};

SYSCALL_DEFINE2(getcwd, char *, buf, size_t, size) {
  if (size < 2) {
    return -ERANGE;
  }

  struct vnode *root = current->fs.root.node;
  struct vnode *cur = current->fs.pwd.node;

  if (cur == rootfs->root) {
    buf[0] = '/';
    buf[1] = '\0';
    return 0;
  }

  int ret = 0;
  struct vnode_path *list = NULL;

  while (cur != root) {
    if (cur->mnt->root == cur) {
      cur = cur->mnt->mountpoint;
    }

    struct vnode_path *p = kmalloc(sizeof(struct vnode_path));
    p->node = cur;
    p->next = list;
    list = p;

    cur = cur->parent;
  }

  /* terminating null byte */
  size_t rsize = size - 1;

  for (struct vnode_path *p = list; p != NULL; p = p->next) {
    if (rsize == 0) {
      ret = -ERANGE;
      break;
    }

    size_t sl = strlen(p->node->name) + 1;
    size_t ml = rsize > sl ? sl : rsize;
    *buf = '/';
    memcpy(buf + 1, p->node->name, ml - 1);
    buf += ml;
    rsize -= ml;
  }

  *buf = '\0';

  struct vnode_path *now = list;
  struct vnode_path *next;

  while (now != NULL) {
    next = now->next;
    kfree(now);
    now = next;
  }

  return ret;
}

SYSCALL_DEFINE3(mount, const char *, source, const char *, target, const char *, fs_name) {
  struct filesystem **fs = find_filesystem(fs_name);
  if (!*fs) {
    return -ENODEV;
  }

  struct vnode *dir = traverse_path(target, 0);
  if (!dir) {
    return -EACCES;
  }

  if (!S_ISDIR(dir->f_mode)) {
    return -ENOTDIR;
  }

  /* CHECK: does this really work ? */
  if (dir->mnt->root == dir) {
    return -EINVAL;
  }

  int ret = (*fs)->vnode_mount(&dir->mnt);
  if (!ret) {
    dir->mnt->mountpoint = dir;
  }

  return ret;
}

SYSCALL_DEFINE1(umount, const char *, target) {
  struct vnode *dir = traverse_path(target, 0);
  if (!dir) {
    return -EACCES;
  }

  if (!S_ISDIR(dir->f_mode)) {
    return -ENOTDIR;
  }

  /* CHECK: does this really work ? */
  if (dir->mnt->root != dir) {
    return -EINVAL;
  }

  dir->mnt->mountpoint->mnt = dir->mnt->mountpoint->parent->mnt;

  /* TODO: release all resources here */

  return 0;
}

void init_mount_tree() {
  struct filesystem **fs = find_filesystem("tmpfs");
  if (!*fs) {
    panic("failed to find tmpfs");
  }

  int ret = (*fs)->vnode_mount(&rootfs);
  if (ret) {
    panic("failed to mount rootfs")
  }

  rootfs->root->parent = rootfs->root;

  current->fs.pwd = (struct path) {
    .mnt = rootfs,
    .node = rootfs->root
  };

  current->fs.root = (struct path) {
    .mnt = rootfs,
    .node = rootfs->root
  };

  populate_rootfs();
}

void init_rootfs() {
  int ret = register_filesystem(&tmpfs);
  if (ret) {
    panic("failed to register tmpfs");
  }

  init_mount_tree();
}



