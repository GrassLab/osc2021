#pragma once

#include <stddef.h>

#include "vfs.h"

#define MAX_FILES_IN_DIR 16
#define TMPFS_BUF_SIZE 4096

struct tmpfs_buf {
  int flag;
  size_t size;
  char buffer[TMPFS_BUF_SIZE];
};

struct tmpfs_fentry {
  char name[10];
  FILE_TYPE type;
  struct vnode* vnode;
  struct tmpfs_fentry* child[MAX_FILES_IN_DIR];
  struct tmpfs_fentry* parent;
  struct tmpfs_buf* buf;
};

struct vnode_operations* tmpfs_v_ops;
struct file_operations* tmpfs_f_ops;

void tmpfs_init();
void tmpfs_set_fentry(struct tmpfs_fentry* fentry, const char* component_name,
                      FILE_TYPE type, struct vnode* vnode);
int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount);
int tmpfs_lookup(struct vnode* dir_node, struct vnode** target,
                 const char* component_name);
int tmpfs_create(struct vnode* dir_node, struct vnode** target,
                 const char* component_name);
int tmpfs_write(struct file* file, const void* buf, size_t len);
int tmpfs_read(struct file* file, void* buf, size_t len);
int tmpfs_list(struct file* file, void* buf, int index);
