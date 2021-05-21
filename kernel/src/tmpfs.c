#include "tmpfs.h"

#include "alloc.h"
#include "printf.h"
#include "string.h"
#include "vfs.h"

void tmpfs_init() {
  tmpfs_v_ops = malloc(sizeof(struct vnode_operations));
  tmpfs_v_ops->lookup = tmpfs_lookup;
  tmpfs_v_ops->create = tmpfs_create;
  tmpfs_f_ops = malloc(sizeof(struct file_operations));
  tmpfs_f_ops->write = tmpfs_write;
  tmpfs_f_ops->read = tmpfs_read;
  tmpfs_f_ops->list = tmpfs_list;
}

void tmpfs_set_fentry(struct tmpfs_fentry* fentry, const char* component_name,
                      FILE_TYPE type, struct vnode* vnode) {
  strcpy(fentry->name, component_name);
  fentry->vnode = vnode;
  fentry->type = type;
  if (fentry->type == FILE_DIRECTORY) {
    for (int i = 0; i < MAX_FILES_IN_DIR; ++i) {
      fentry->child[i] =
          (struct tmpfs_fentry*)malloc(sizeof(struct tmpfs_fentry));
      fentry->child[i]->name[0] = 0;
      fentry->child[i]->type = FILE_NONE;
      fentry->child[i]->parent = fentry;
    }
  } else if (fentry->type == FILE_REGULAR) {
    fentry->buf = malloc(sizeof(struct tmpfs_buf));
    fentry->buf->size = 0;
  }
}

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount) {
  struct tmpfs_fentry* root_fentry =
      (struct tmpfs_fentry*)malloc(sizeof(struct tmpfs_fentry));
  struct vnode* vnode = (struct vnode*)malloc(sizeof(struct vnode));
  vnode->v_ops = tmpfs_v_ops;
  vnode->f_ops = tmpfs_f_ops;
  vnode->internal = (void*)root_fentry;
  tmpfs_set_fentry(root_fentry, "/", FILE_DIRECTORY, vnode);
  mount->fs = fs;
  mount->root = vnode;
  return 1;
}

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target,
                 const char* component_name) {
  for (int i = 0; i < MAX_FILES_IN_DIR; i++) {
    struct tmpfs_fentry* fentry =
        ((struct tmpfs_fentry*)dir_node->internal)->child[i];
    if (!strcmp(fentry->name, component_name)) {
      *target = fentry->vnode;
      return 1;
    }
  }
  return 0;
}

int tmpfs_create(struct vnode* dir_node, struct vnode** target,
                 const char* component_name) {
  for (int i = 0; i < MAX_FILES_IN_DIR; i++) {
    struct tmpfs_fentry* fentry =
        ((struct tmpfs_fentry*)dir_node->internal)->child[i];
    if (fentry->type == FILE_NONE) {
      struct vnode* vnode = malloc(sizeof(struct vnode));
      vnode->v_ops = dir_node->v_ops;
      vnode->f_ops = dir_node->f_ops;
      vnode->internal = fentry;
      tmpfs_set_fentry(fentry, component_name, FILE_REGULAR, vnode);
      *target = fentry->vnode;
      return 1;
    }
  }
  return -1;
}

int tmpfs_write(struct file* file, const void* buf, size_t len) {
  for (size_t i = 0; i < len; i++) {
    struct tmpfs_fentry* fentry = (struct tmpfs_fentry*)file->vnode->internal;
    fentry->buf->buffer[file->f_pos++] = ((char*)buf)[i];
    if (fentry->buf->size < file->f_pos) {
      fentry->buf->size = file->f_pos;
    }
  }
  return len;
}

int tmpfs_read(struct file* file, void* buf, size_t len) {
  size_t read_len = 0;
  for (size_t i = 0; i < len; i++) {
    struct tmpfs_fentry* fentry = (struct tmpfs_fentry*)file->vnode->internal;
    ((char*)buf)[i] = fentry->buf->buffer[file->f_pos++];
    read_len++;
    if (read_len == fentry->buf->size) {
      break;
    }
  }
  return read_len;
}

int tmpfs_list(struct file* file, void* buf, int index) {
  struct tmpfs_fentry* fentry = (struct tmpfs_fentry*)file->vnode->internal;
  if (fentry->type != FILE_DIRECTORY) return -1;
  if (index >= MAX_FILES_IN_DIR) return -1;

  if (fentry->child[index]->type == FILE_NONE) return 0;
  strcpy((char*)buf, fentry->child[index]->name);
  return fentry->child[index]->buf->size;
}
