#pragma once
#include "vfs.h"
#define TMPFS_CHILD_MAX 8
#define TMPFS_BUF_SIZE 512
extern struct file_operations *tmpfs_file_operations;
extern struct vnode_operations *tmpfs_vnode_operations;

typedef struct tmpfs_buf
{
    int flag, size;
    char buffer[TMPFS_BUF_SIZE];
} tmpfs_buf;
typedef struct tmpfs_entry
{
    char name[16];
    struct tmpfs_entry *list[TMPFS_CHILD_MAX];
    struct tmpfs_entry *parent;
    FILE_TYPE type;
    struct vnode *vnode;
    tmpfs_buf *buf;

} tmpfs_entry;

int tmpfs_write(struct file *file, const void *buf, size_t len);
int tmpfs_read(struct file *file, void *buf, size_t len);
int tmpfs_list(struct file *file, void *buf, int id);
int tmpfs_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
int tmpfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name);
int tmpfs_setup_mount(struct filesystem *fs, struct mount *mount);
void tmpfs_set_entry(tmpfs_entry *entry, struct vnode *v_node, const char *component_name);