#include "fs/tmpfs.h"
#include "fs/vfs.h"

#include "mm.h"
#include "stdint.h"
#include "string.h"
#include "uart.h"
#include <stddef.h>

// TODO: use linked-list to replace staticaly allocate space
#define TMPFS_DIR_CAPACITY 10

#define TMPFS_NODE_TYPE_DIR 1
#define TMPFS_NODE_TYPE_FILE 2

// Private content for vnode in tmpfs
typedef struct {
  char *name;
  uint8_t node_type;
  size_t size;

  // Differernt node could access differnt attribute
  union {
    // Dir node
    struct {
      struct vnode **children;
    };
    // File node
    struct {};
  };
} Content;

#define content_ptr(vnode) (Content *)((vnode)->internal)

// lazy init
int tmpfs_initialized = false;
struct vnode_operations *tmpfs_v_ops = NULL;
struct file_operations *tmpfs_f_ops = NULL;

struct filesystem tmpfs = {
    .name = "tmpfs", .setup_mount = tmpfs_setup_mount, .next = NULL};

// Create & Initialize a empty vnode for TMPFS
static struct vnode *create_vnode(char *name, uint8_t node_type);

static void build_root_tree(struct vnode *root_dir);

// bind operations
int tmpfs_init() {
  tmpfs_v_ops = kalloc(sizeof(struct vnode_operations));
  tmpfs_v_ops->lookup = tmpfs_lookup;
  tmpfs_v_ops->create = tmpfs_create;

  tmpfs_f_ops = kalloc(sizeof(struct file_operations));
  tmpfs_f_ops->read = tmpfs_read;
  tmpfs_f_ops->write = tmpfs_write;
  return 0;
}

static struct vnode *create_vnode(char *name, uint8_t node_type) {
  struct vnode *node = (struct vnode *)kalloc(sizeof(struct vnode));

  // This node is not mounted by other directory
  node->mnt = NULL;
  node->v_ops = tmpfs_v_ops;
  node->f_ops = tmpfs_f_ops;

  Content *cnt = kalloc(sizeof(Content));
  {
    cnt->name = (char *)kalloc(sizeof(char) * strlen(name));
    strcpy(cnt->name, name);
    cnt->node_type = node_type;
    cnt->size = 0;
    if (node_type == TMPFS_NODE_TYPE_DIR) {
      cnt->children = kalloc(sizeof(struct vnode *) * TMPFS_DIR_CAPACITY);
    }
  }
  node->internal = cnt;
  return node;
}

void build_root_tree(struct vnode *root_dir) {
  // Create a fake nodes for prototyping
  struct vnode *hello_dir = create_vnode("hello", TMPFS_NODE_TYPE_DIR);
  {
    Content *cnt = root_dir->internal;
    cnt->size++;
    cnt->children[0] = hello_dir;
  }

  struct vnode *world_dir = create_vnode("world", TMPFS_NODE_TYPE_DIR);
  {
    Content *cnt = hello_dir->internal;
    cnt->size++;
    cnt->children[0] = world_dir;
  }

  struct vnode *ian_file = create_vnode("ian", TMPFS_NODE_TYPE_FILE);
  {
    Content *cnt = world_dir->internal;
    cnt->size++;
    cnt->children[0] = ian_file;
  }
};

int tmpfs_setup_mount(struct filesystem *fs, struct mount *mount) {
  uart_println("tmpf setup mount");
  if (tmpfs_initialized == false) {
    tmpfs_init();
    tmpfs_initialized = true;
  }

  struct vnode *root = create_vnode("/", TMPFS_NODE_TYPE_DIR);
  build_root_tree(root);

  // Officially mount this root vnode onto the mount point
  mount->root = root;
  return 0;
}

int tmpfs_write(struct file *f, const void *buf, unsigned long len) {
  // TODO
  return 0;
}
int tmpfs_read(struct file *f, void *buf, unsigned long len) {
  // TODO
  return 0;
}
int tmpfs_lookup(struct vnode *dir_node, struct vnode **target,
                 const char *component_name) {
  Content *dir_content = content_ptr(dir_node);
  uart_println(" tmpfs lookup node:`%s`, path: `%s`", dir_content->name,
               component_name);

  Content *child_content;
  struct vnode *child_vnode;
  for (int i = 0; i < dir_content->size; i++) {
    child_vnode = dir_content->children[i];
    child_content = content_ptr(child_vnode);
    if (strcmp(child_content->name, component_name) == 0) {
      *target = child_vnode;
      return 0;
      ;
    }
  }
  *target = NULL;
  return 0;
}
int tmpfs_create(struct vnode *dir_node, struct vnode **target,
                 const char *component_name) {
  // TODO
  return 0;
}
