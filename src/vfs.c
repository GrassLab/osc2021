#include "vfs.h"
#include "mm.h"
#include "tmpfs.h"
#include "string.h"
#include "io.h"
#include "cpio.h"

extern struct mount* rootfs;

#define OFT_SIZE 25
struct file open_file_table[25];
#define MOUNT_TABLE_SIE 10
struct mount mount_table[MOUNT_TABLE_SIE];


void init_vfs () {
    for (int i = 0; i < OFT_SIZE; i++) {
        open_file_table[i].flags = F_CLOSE;
    }

    for (int i = 0; i < MOUNT_TABLE_SIE; i++) {
        mount_table[i].fs = NULL;
        mount_table[i].root = NULL;
    }

    rootfs = &mount_table[0];
    struct vnode *root = m_malloc(sizeof(struct vnode));
    struct filesystem *fs = m_malloc(sizeof(struct filesystem));

    mount_table[0] = (struct mount) {
        .root = root,
        .fs = fs,
    };

    struct file_operations *fop = m_malloc(sizeof(struct file_operations));
    *fop = (struct file_operations) {
        .write = tmpfs_write,
        .read = tmpfs_read,
        .flush = tmpfs_file_flush,
    };

    struct vnode_operations *vop = m_malloc(sizeof(struct vnode_operations));
    *vop = (struct vnode_operations) {
        .lookup = tmpfs_lookup,
        .create = tmpfs_create,
        .flush = tmpfs_vnode_flush,
    };

    *root = (struct vnode) {
        .mount = &mount_table[0],
        .v_ops = vop,
        .f_ops = fop,
        .childs = NULL,
        .name = strdup(""),
        .internal = NULL,
        .type = VNODE_DIR,
    };

    /* populate initrampfs */
    cpio_vfs_mount();
}

void vfs_ls (const char *path) {
    const char *ptr = path;
    if (path[0] == '/')
        ptr = &path[1];

    struct vnode *root = rootfs->root, *tmp;
    if (strlen(ptr))
        root->v_ops->lookup(root, &tmp, ptr);
    else
        tmp = root;

    if (!tmp) {
        kprintf("No such folder: %s\n", path);
        return;
    }

    for (struct vnode_child *ptr = tmp->childs; ptr; ptr = ptr->next) {
        kprintf("%s ", ptr->child->name);
    }
    kprintf("\n");
}

int vfs_touch (const char *path) {
    const char *ptr = path;
    if (path[0] == '/')
        ptr = &path[1];

    struct vnode *root = rootfs->root, *tmp;
    /* fail to create folder */
    if (root->v_ops->create(root, &tmp, ptr)) {
        kprintf("fail to create folder\n");
        return -1;
    }

    tmp->type = VNODE_FILE;

    // update data in devices
    tmp->v_ops->flush(tmp);

    return 0;
}

int vfs_mkdir (const char *path) {
    const char *ptr = path;
    if (path[0] == '/')
        ptr = &path[1];

    struct vnode *root = rootfs->root, *tmp;
    /* fail to create folder */
    if (root->v_ops->create(root, &tmp, ptr)) {
        kprintf("fail to create file\n");
        return -1;
    }

    tmp->type = VNODE_DIR;

    // update data in devices
    tmp->v_ops->flush(tmp);

    return 0;
}

int register_filesystem(struct filesystem* fs) {
    int i;
    for (i = 0; i < MOUNT_TABLE_SIE; i++)
        if (!mount_table[i].fs) break;

    if (mount_table[i].fs)
        return -1;

    fs->setup_mount(mount_table[0].fs, &mount_table[i]);

    struct vnode_child *list = m_malloc(sizeof(struct vnode_child));
    list->child = mount_table[i].root;
    list->next = rootfs->root->childs;
    rootfs->root->childs = list;
    return 0;
}

struct file* vfs_open(const char* pathname, int flags) {
    const char *ptr = pathname;
    if (pathname[0] == '/')
        ptr = &pathname[1];

    struct vnode *root = rootfs->root, *tmp;
    /* create new file */
    if (flags == O_CREAT) {
        if (strlen(ptr))
            root->v_ops->lookup(root, &tmp, ptr);
        if (!tmp)
            vfs_touch(pathname);
    }

    if (strlen(ptr))
        root->v_ops->lookup(root, &tmp, ptr);
    else
        tmp = root;

    /* no such file */
    if (!tmp) {
        kprintf("no such file: %s", pathname);
        return NULL;
    }
    /* cannot open a folder */
    else if (tmp->type == VNODE_DIR) {
        kprintf("cannot open folder\n");
        return NULL;
    }


    int i;
    struct file *ret = NULL;
    for (i = 0; i < OFT_SIZE; i++) {
        if (open_file_table[i].flags == F_CLOSE) {
            ret = &open_file_table[i];
            ret->flags = flags;
            ret->vnode = tmp;
            ret->f_ops = tmp->f_ops;
            ret->f_pos = 0;
            ret->internal = NULL;
            break;
        }
    }
    return ret;
}

int vfs_close(struct file* file) {
    if (!file) return -1;
    *file = (struct file) {
        .vnode = NULL,
        .f_pos = 0,
        .f_ops = NULL,
        .flags = F_CLOSE,
    };
    return 0;
}

int vfs_write(struct file* file, const void* buf, u32 len) {
    // TODO: check file size before writing content
    return file->f_ops->write(file, buf, len);
}
int vfs_read(struct file* file, void* buf, u32 len) {
    // TODO: check file size before reading content
    return file->f_ops->read(file, buf, len);
}
