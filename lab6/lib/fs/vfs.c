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

struct filesystem *filesystem_list;

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

/* TODO: handle passing another fs while traversing */
struct vnode *traverse_path(const char *pathname, int last_dir) {
  struct vnode *now;
  char *p, *pp, *name;

  p = pp = strdup(pathname);

  if (p[0] == '/') {
    now = current->fs.root.node;
    p++;
  } else {
    now = current->fs.pwd.node;
  }

  struct vnode *target = now;

  while (p) {
    name = strsep(&p, "/");

    if (last_dir && !p) {
      break;
    }
    /* TODO: we have to define what should be return when path escape the mountpoint */
    int ret = now->v_ops->lookup(now, &target, name);
    if (ret || !target) {
      target = NULL;
      break;
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

  /* TODO: check if path is valid */
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

    int ret = dir->v_ops->create(dir, name);
    if (ret) {
      return NULL;
    }

    struct vnode *target;
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
  if (!file->refcnt) {
    kfree(file);
  }

  return 0;
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

void init_mount_tree() {
  struct filesystem **fs = find_filesystem("tmpfs");
  if (!*fs) {
    panic("failed to find tmpfs");
  }

  struct vnode *root = kcalloc(sizeof(struct vnode));
  root->name = "/";
  root->nodes = LIST_HEAD_INIT(root->nodes);
  root->subnodes = LIST_HEAD_INIT(root->subnodes);
  root->f_mode = S_IFDIR;

  int ret = (*fs)->vnode_mount(root);
  if (ret) {
    panic("failed to mount rootfs")
  }

  current->fs.pwd = (struct path) {
    .mnt = root->mnt,
    .node = root
  };

  current->fs.root = (struct path) {
    .mnt = root->mnt,
    .node = root
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



