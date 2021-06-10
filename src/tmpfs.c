#include "tmpfs.h"
#include "mm.h"
#include "utils.h"

typedef struct {
    uint32_t size;
    void     *addr;
} tmpfs_file_t;

vnode_operations_t tmpfs_vop;
file_operations_t tmpfs_fop;

static void tmpfs_umount_operation(vnode_t *file) {
    tmpfs_file_t *f = (tmpfs_file_t*)file->internal;
    kfree(f->addr);
    kfree(f);
}

static void tmpfs_vnode_init(vnode_t *vnode, const char *name) {
    vfs_vnode_init(vnode, name);
    vnode->v_ops = &tmpfs_vop;
    vnode->f_ops = &tmpfs_fop;
}

static void tmpfs_create_file(vnode_t *dir_node, vnode_t **target, const char *file_name) {
    vnode_t *new = kmalloc(sizeof(vnode_t));
    tmpfs_file_t *file = kmalloc(sizeof(tmpfs_file_t));
    file->size = 0;
    file->addr = kmalloc(MAX_FILE_SIZE);
    tmpfs_vnode_init(new, file_name);
    new->internal = file;
    list_add_tail(&dir_node->node, &new->node);
    *target = new;
}

static void tmpfs_create_dir(vnode_t *dir_node, const char *dir_name) {
    vnode_t *new = kmalloc(sizeof(vnode_t));
    tmpfs_vnode_init(new, dir_name);
    new->dentry.parent = dir_node;
    list_add_tail(&dir_node->dentry.subdir, &new->dentry.sibling);
}

static int tmpfs_read(file_t *file, void *buf, size_t len) {
    tmpfs_file_t *f = (tmpfs_file_t*)file->vnode->internal;
    if ((file->f_pos + len) > f->size)
        len = f->size - file->f_pos;
    memcpy(buf, (f->addr + file->f_pos), len);
    file->f_pos += len;
    return len;
}

static int tmpfs_write(file_t *file, const void *buf, size_t len) {
    tmpfs_file_t *f = (tmpfs_file_t*)file->vnode->internal;
    if ((file->f_pos + len) > MAX_FILE_SIZE) {
        len = MAX_FILE_SIZE - file->f_pos;
        if (!len)
            return -1;
    }
    memcpy((f->addr + file->f_pos), buf, len);
    if ((file->f_pos + len) > f->size)
        f->size += len - (f->size - file->f_pos);
    file->f_pos += len;
    return len;
}

void tmpfs_setup_mount(mount_t *mount, const char *name) {
    char *fs_name = kmalloc(strlen(name) + 1);
    strcpy(fs_name, name);
    mount->fs_name = fs_name;
    mount->umount_ops = &tmpfs_umount_operation;

    mount->root = kmalloc(sizeof(vnode_t));
    strcpy(mount->root->name, "/");
    mount->root->dentry.mount = NULL;
    mount->root->dentry.parent = NULL;
    list_init(&mount->root->dentry.sibling);
    list_init(&mount->root->dentry.subdir);
    mount->root->v_ops = &tmpfs_vop;
    mount->root->f_ops = &tmpfs_fop;
    mount->root->internal = NULL;
    list_init(&mount->root->node);
}

void tmpfs_init() {
    tmpfs_vop.create_file = &tmpfs_create_file;
    tmpfs_vop.create_dir = &tmpfs_create_dir;
    tmpfs_fop.write = &tmpfs_write;
    tmpfs_fop.read = &tmpfs_read;
}
