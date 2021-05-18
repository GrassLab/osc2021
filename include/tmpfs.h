#ifndef	_TMPFS_H
#define	_TMPFS_H

#define TMPFS_ERROR -1
#define TMPFS_READ_ERROR -1
#define TMPFS_WRITE_OVERFLOW_ERROR -1
#define TMP_DEFAULT_FILE_SIZE 4096

struct tmpfs_internal {
    size_t max_buf_size;
    char *buf;
};

int tmpfs_setup_mount();
int tmpfs_register();
struct dentry* tmpfs_create_dentry(struct dentry *parent, const char *name, int type);
struct vnode* tmpfs_create_vnode(struct dentry *dentry, int type);

int tmpfs_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
int tmpfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name);
int tmpfs_write(struct file *file, const void *buf, size_t len);
int tmpfs_read(struct file *file, void *buf, size_t len);
int tmpfs_mkdir(struct vnode *parent, const char *component_name);

#endif