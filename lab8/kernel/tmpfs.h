#ifndef __TMPFS_H
#define __TMPFS_H

#include <stdint.h>
#include "vfs.h"

#define TFS_FILE 0b001
#define TFS_DIR 0b010
#define TFS_DFILE 0b100

typedef struct {
    struct vnode *next;
    uint64_t mode;
    char *name;
    char *content;
    size_t content_size;  //1 = 0
} tmpfs_node;

static int setup_mount(struct filesystem *fs, struct mount *mount);
static int lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
static int create(struct vnode* dir_node, struct vnode **target, const char *component_name, uint64_t mode);
static int create_file(struct vnode* dir_node, struct vnode **target, const char *component_name);
static int create_dir(struct vnode* dir_node, const char *component_name);
static int write(struct file *file, const void *buf, size_t len);
static int read(struct file *file, void *buf, size_t len);
static size_t filesize(struct file *file);
static void list(struct vnode *dir_node);
static void node_name(struct vnode *vnode, char *buf);
static void *content(struct vnode *vnode);

static int init_dir(struct vnode *child, struct vnode *parent);
static void set_parent(struct vnode *mount, struct vnode *mountpoint);

#endif
