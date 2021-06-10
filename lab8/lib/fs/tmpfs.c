#include <stddef.h>
#include <fs/vfs.h>
#include <mm.h>
#include <string.h>
#include <list.h>
#include <stat.h>
#include <file.h>
#include <asm/errno.h>

#define TMPFS_NORMAL_FILE (1 << 0)
#define TMPFS_DIRECTORY (1 << 1)

static int mount_tmpfs(struct mount **mountpoint);

static int tmpfs_lookup(struct vnode* dir_node, struct vnode **target, const char *component_name);
static int tmpfs_create(struct vnode* dir_node, const char *component_name);
static int tmpfs_unlink(struct vnode* dir_node, const char *component_name);
static int tmpfs_mkdir(struct vnode* dir_node, const char *component_name);
static int tmpfs_rmdir(struct vnode* dir_node, const char *component_name);

static ssize_t tmpfs_read(struct file *file, void *buf, size_t len);
static ssize_t tmpfs_write(struct file *file, const void *buf, size_t len);
static int tmpfs_fsync(struct file *file);

struct filesystem tmpfs = {
  .name = "tmpfs",
  .vnode_mount = &mount_tmpfs
};

static struct vnode_operations tmpfs_v_ops = {
  .lookup = &tmpfs_lookup,
  .create = &tmpfs_create,
  .unlink = &tmpfs_unlink,
  .mkdir = &tmpfs_mkdir,
  .rmdir = &tmpfs_rmdir
};

static struct file_operations tmpfs_f_ops = {
  .read = &tmpfs_read,
  .write = &tmpfs_write,
  .fsync = &tmpfs_fsync
};

/* TODO: add enum for return value */

static ssize_t tmpfs_read(struct file *file, void *buf, size_t len) {
    ssize_t size = len;

    if (size < 0) {
        return -EOVERFLOW;
    }

    ssize_t mx = file->vnode->size - file->f_pos;
    if (len > mx) {
        size = mx;
    }

    memcpy(buf, (char *)file->vnode->internal + file->f_pos, size);
    file->f_pos += size;

    return size;
}

static ssize_t tmpfs_write(struct file *file, const void *buf, size_t len) {
    ssize_t size = len;

    if (size < 0) {
        return -EOVERFLOW;
    }

    /* TODO: since kmalloc can allocate more memory space than user requested,
     * we should use that size as capacity to reduce reallocation */
    size_t total_size = file->f_pos + len;

    if (total_size > file->vnode->capacity) {
        void *content = kmalloc(total_size);
        file->vnode->capacity = get_alloc_size(content);

        memcpy(content, file->vnode->internal, file->f_pos);
        memcpy((char *)content + file->f_pos, buf, len);
        kfree(file->vnode->internal);

        file->vnode->internal = content;
    } else {
        memcpy((char *)file->vnode->internal + file->f_pos, buf, len);
    }

    if (total_size > file->vnode->size) {
        file->vnode->size = total_size;
    }

    file->f_pos += len;

    return size;
}

static int tmpfs_fsync(struct file *file) {
    return 0;
}

static int tmpfs_lookup(struct vnode* dir_node, struct vnode **target, const char *component_name) {
    struct list_head *p;
    struct vnode *v;

    *target = NULL;

    list_for_each(p, &dir_node->subnodes) {
        v = list_entry(p, struct vnode, nodes);
        if (!strcmp(v->name, component_name)) {
            *target = v;
        }
    }

    return 0;
}

/* create a normal file */
static int tmpfs_create(struct vnode* dir_node, const char *component_name) {
    struct vnode *v;
    tmpfs_lookup(dir_node, &v, component_name);

    if (v) {
        return -1;
    }

    struct vnode *node = kmalloc(sizeof(struct vnode));
    node->mnt = dir_node->mnt;
    node->name = strdup(component_name);
    node->parent = dir_node;
    node->subnodes = LIST_HEAD_INIT(node->subnodes);
    node->flags = TMPFS_NORMAL_FILE;
    node->f_mode = S_IFREG;
    node->v_ops = &tmpfs_v_ops;
    node->f_ops = &tmpfs_f_ops;
    node->size = 0;
    node->capacity = 0;
    node->internal = NULL;
    insert_head(&dir_node->subnodes, &node->nodes);

    return 0;
}

/* unlink a normal file */
static int tmpfs_unlink(struct vnode* dir_node, const char *component_name) {
    struct vnode *node;
    tmpfs_lookup(dir_node, &node, component_name);

    if (!node) {
        return -1;
    }

    if (node->flags & TMPFS_NORMAL_FILE == 0) {
        return -2;
    }

    kfree(node->name);
    kfree(node->internal); /* TODO: fix this shit */
    unlink(&node->nodes);

    return 0;
}

/* create a directory */
static int tmpfs_mkdir(struct vnode* dir_node, const char *component_name) {
    struct vnode *v;
    tmpfs_lookup(dir_node, &v, component_name);

    if (v) {
        return -1;
    }

    struct vnode *node = kmalloc(sizeof(struct vnode));
    node->mnt = dir_node->mnt;
    node->name = strdup(component_name);
    node->parent = dir_node;
    node->subnodes = LIST_HEAD_INIT(node->subnodes);
    node->flags = TMPFS_DIRECTORY;
    node->f_mode = S_IFDIR;
    node->v_ops = &tmpfs_v_ops;
    node->f_ops = &tmpfs_f_ops;
    node->internal = (void *)0xdeadbeefdeadbeef;
    insert_head(&dir_node->subnodes, &node->nodes);

    return 0;
}

/* delete a directory */
static int tmpfs_rmdir(struct vnode* dir_node, const char *component_name) {
    struct vnode *node;
    tmpfs_lookup(dir_node, &node, component_name);

    if (!node) {
        return -1;
    }

    if (node->flags & TMPFS_DIRECTORY == 0) {
        return -2;
    }

    if (!list_empty(&node->subnodes)) {
        return -3;
    }

    kfree(node->name);
    unlink(&node->nodes);

    return 0;
}

static int mount_tmpfs(struct mount **mountpoint) {
    struct mount *mnt = kmalloc(sizeof(struct mount));
    if (!mnt) {
        return -ENOSPC;
    }

    struct vnode *root = kcalloc(sizeof(struct vnode));
    if (!root) {
        kfree(mnt);
        return -ENOSPC;
    }

    root->name = "/";
    root->parent = root;
    root->nodes = LIST_HEAD_INIT(root->nodes);
    root->subnodes = LIST_HEAD_INIT(root->subnodes);
    root->f_mode = S_IFDIR;
    root->mnt = mnt;
    root->v_ops = &tmpfs_v_ops;
    root->f_ops = &tmpfs_f_ops;
    root->flags = TMPFS_DIRECTORY;

    mnt->root = root;
    mnt->fs = &tmpfs;

    *mountpoint = mnt;
    return 0;
}