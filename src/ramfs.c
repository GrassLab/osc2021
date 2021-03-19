#include "ramfs.h"

#include "io.h"
#include "mem.h"
#include "util.h"

#define pad4(x) ((((x) + 3) / 4) * 4)

#define MAX_DIR 16

#define FILETYPE_MASK 0xF000
#define FILETYPE_FILE 0x8000
#define FILETYPE_DIR 0x4000

#define IS_FILE(ft) ((ft & FILETYPE_MASK) == FILETYPE_FILE)
#define IS_DIR(ft) ((ft & FILETYPE_MASK) == FILETYPE_DIR)

int contain_dir(char *path) {
  int last = 0;
  while (*path) {
    last++;
    if (*path == '/') {
      *path = 0;
      return last;
    }
    path++;
  }
  return 0;
}

struct ramfs_inode {
  struct ramfs_inode *parent;
  unsigned int filetype;
  char *name;
  unsigned int size;
  void *data;
};

struct ramfs_child_list {
  struct ramfs_inode *child[MAX_DIR];
  struct ramfs_child_list *next;
};

struct ramfs_file_data {
  char data[4080];
  struct ramfs_file_data *next;
};

struct ramfs_inode *new_ramfs_inode(const char *name, unsigned int filetype,
                                    struct ramfs_inode *parent) {
  struct ramfs_inode *node = kmalloc(sizeof(struct ramfs_inode));
  unsigned int namesize = strlen(name);
  node->name = kmalloc(sizeof(char) * (namesize + 1));
  strcpy(node->name, name);

  node->parent = parent;
  node->filetype = filetype;
  node->size = 0;
  if (IS_DIR(filetype)) {
    node->data = kmalloc(sizeof(struct ramfs_child_list));
    puts("new dir ");
    puts(node->name);
    putc('\n');
  } else if (IS_FILE(filetype)) {
    puts("new file ");
    puts(node->name);
    putc('\n');
    node->data = kmalloc(sizeof(struct ramfs_file_data));
  }
  return node;
}

struct ramfs_sb {
  struct ramfs_inode *root_dir;
};

int ramfs_mkdir(const char *path, struct dentry *dir) {
  puts("mkdir ");
  puts(path);
  putc('\n');
  struct ramfs_inode *internal = (struct ramfs_inode *)(dir->node->internal);
  unsigned int dir_size = internal->size;
  struct ramfs_child_list *vac_list =
      (struct ramfs_child_list *)(internal->data);
  while (dir_size >= MAX_DIR) {
    vac_list = vac_list->next;
    dir_size -= MAX_DIR;
  }
  // puts(path);
  // putc('\n');
  vac_list->child[dir_size] = new_ramfs_inode(path, FILETYPE_DIR, internal);
  internal->size++;
  puts(vac_list->child[dir_size]->name);
  putc('\n');
  return 0;
}
int ramfs_chdir(const char *path, struct dentry *dir) {
  puts("chdir ");
  puts(path);
  putc('\n');
  if (strcmp(path, "..") == 0) {
    dir->node->internal = ((struct ramfs_inode *)(dir->node->internal))->parent;
  } else if (strcmp(path, ".")) {
    struct ramfs_inode *internal = (struct ramfs_inode *)(dir->node->internal);
    unsigned int dir_size = internal->size;
    struct ramfs_child_list *vac_list =
        (struct ramfs_child_list *)(internal->data);
    for (int i = 0; i < dir_size; i++) {
      if (strcmp(vac_list->child[i % MAX_DIR]->name, path) == 0) {
        dir->node->internal = vac_list->child[i % MAX_DIR];
        return 0;
      }
      if (i % MAX_DIR == MAX_DIR - 1) {
        vac_list = vac_list->next;
      }
    }
    puts("fail chdir\n");
    return 1;
  }

  return 0;
}

int ramfs_rmdir(const char *path, struct dentry *dir) { return 0; }

int ramfs_open(const char *path, struct dentry *dir, struct file *f) {
  // f->dir_node = dir->node;
  // f->file_node = dir->node;
  f->file_node = kmalloc(sizeof(struct inode));
  f->file_node->sb = dir->node->sb;

  struct ramfs_inode *node_itr;
  if (*path == '/') {
    node_itr = ((struct ramfs_sb *)(dir->node->sb->internal))->root_dir;
    path++;
  } else {
    node_itr = dir->node->internal;
  }
  unsigned int pathsize = strlen(path);
  char *p = kmalloc(sizeof(char) * (pathsize + 1));
  strcpy(p, path);
  int path_chop;
  while ((path_chop = contain_dir(p)) != 0) {
    // puts("chop\n");
    if (strcmp(p, "..") == 0) {
      node_itr = node_itr->parent;
    } else if (strcmp(path, ".")) {
      unsigned int dir_size = node_itr->size;
      struct ramfs_child_list *vac_list =
          (struct ramfs_child_list *)(node_itr->data);
      for (int i = 0; i < dir_size; i++) {
        if (strcmp(vac_list->child[i % MAX_DIR]->name, p) == 0) {
          node_itr = vac_list->child[i % MAX_DIR];
          break;
        }
        if (i % MAX_DIR == MAX_DIR - 1) {
          vac_list = vac_list->next;
        }
      }
    }
    p = p + path_chop;
  }
  // puts("yep2\n");
  int find = 0;
  unsigned int dir_size = node_itr->size;
  struct ramfs_child_list *vac_list =
      (struct ramfs_child_list *)(node_itr->data);
  for (int i = 0; i < dir_size; i++) {
    if (strcmp(vac_list->child[i % MAX_DIR]->name, p) == 0) {
      node_itr = vac_list->child[i % MAX_DIR];
      find = 1;
      break;
    }
    if (i % MAX_DIR == MAX_DIR - 1) {
      vac_list = vac_list->next;
    }
  }
  if (find == 0) {
    unsigned int dir_size = node_itr->size;
    struct ramfs_child_list *vac_list =
        (struct ramfs_child_list *)(node_itr->data);
    while (dir_size >= MAX_DIR) {
      vac_list = vac_list->next;
      dir_size -= MAX_DIR;
    }
    // puts("yep\n");
    vac_list->child[dir_size] = new_ramfs_inode(p, FILETYPE_FILE, node_itr);
    node_itr->size++;
    f->file_node->internal = vac_list->child[dir_size];
  } else {
    f->file_node->internal = node_itr;
  }
  puts("open ");
  puts(path);
  putc('\n');
  return 0;
}

int ramfs_read(struct file *f, char *buf, size_t len) {
  struct ramfs_inode *node = (struct ramfs_inode *)(f->file_node->internal);
  struct ramfs_file_data *file_itr = (struct ramfs_file_data *)(node->data);
  if (len > node->size) {
    len = node->size;
  }
  int tt = len;

  while (len > 0) {
    if (len > 4080) {
      memcpy(buf, file_itr->data, 4080);
      len -= 4080;
      buf += 4080;
      file_itr = file_itr->next;
    } else {
      memcpy(buf, file_itr->data, len);
      len = 0;
    }
  }
  return tt;
}

int ramfs_write(struct file *f, const char *buf, size_t len) {
  puts("writee\n");
  struct ramfs_inode *node = (struct ramfs_inode *)(f->file_node->internal);
  struct ramfs_file_data *file_itr = (struct ramfs_file_data *)(node->data);
  node->size = len;
  while (len > 0) {
    if (len > 4080) {
      memcpy(file_itr->data, buf, 4080);
      len -= 4080;
      buf += 4080;
      file_itr->next = kmalloc(sizeof(struct ramfs_file_data));
      file_itr = file_itr->next;
    } else {
      memcpy(file_itr->data, buf, len);
      len = 0;
    }
  }
  puts("write ");
  puts_n((char *)node->data, 10);
  putc('\n');
  return 0;
}

int ramfs_close(struct file *f) {
  puts("close ");
  putc('\n');
  return 0;
}

int ramfs_get_dir(const char *path, struct dentry *dir,
                  struct super_block *sb) {
  struct inode *new_inode = kmalloc(sizeof(struct inode));
  dir->node = new_inode;
  struct ramfs_inode *node = ((struct ramfs_sb *)(sb->internal))->root_dir;
  // puts(node->name);
  new_inode->internal = node;
  new_inode->sb = sb;
  new_inode->ref_count = 1;
  // puts("get dir\n");

  return 0;
}
int ramfs_close_dir(struct dentry *dir) { return 0; }

struct super_block *new_ramfs() {
  struct super_block *new_block = kmalloc(sizeof(struct super_block));
  new_block->fs_name = "ramfs";
  new_block->mnt_l = NULL;
  new_block->sb_ops.mkdir = &ramfs_mkdir;
  new_block->sb_ops.chdir = &ramfs_chdir;
  new_block->sb_ops.rmdir = &ramfs_rmdir;
  new_block->sb_ops.open = &ramfs_open;
  new_block->sb_ops.read = &ramfs_read;
  new_block->sb_ops.write = &ramfs_write;
  new_block->sb_ops.close = &ramfs_close;
  new_block->sb_ops.get_dir = &ramfs_get_dir;
  new_block->sb_ops.close_dir = &ramfs_close_dir;
  struct ramfs_sb *sb_in = kmalloc(sizeof(struct ramfs_sb));
  new_block->internal = (void *)sb_in;
  sb_in->root_dir = new_ramfs_inode("/", FILETYPE_DIR, NULL);
  puts("new ramfs\n");
  return new_block;
}

int is_child(struct cpio_newc_header **cur_pos,
             struct cpio_newc_header *parent) {
  size_t parent_namesize = strlen(parent->data);
  if (parent_namesize == 0) {
    return 1;
  }
  if (strcmp_n(parent->data, (*cur_pos)->data, parent_namesize) == 0) {
    if ((*cur_pos)->data[parent_namesize] == '/') {
      return 1;
    }
  }
  return 0;
}

int _parse_initramfs(struct dentry *dir, struct cpio_newc_header **cur_pos,
                     struct cpio_newc_header *parent) {
  size_t relate_st = strlen(parent->data);
  if (relate_st != 0) {
    relate_st++;
  }

  while (strcmp_n((*cur_pos)->c_magic, "070701", 6) == 0) {
    if (is_child(cur_pos, parent) == 0) {
      return 0;
    }
    unsigned long filesize = atol_n((*cur_pos)->c_filesize, 8, 16);
    unsigned long namesize = atol_n((*cur_pos)->c_namesize, 8, 16);
    unsigned long mode = atol_n((*cur_pos)->c_mode, 8, 16);
    if (mode == 0 && strcmp((*cur_pos)->data, "TRAILER!!!") == 0) {
      return 1;
    }
    // puts((*cur_pos)->data);
    // putc('\n');

    struct cpio_newc_header *cur = (*cur_pos);
    (*cur_pos) = (struct cpio_newc_header *)&(
        (*cur_pos)->data[2 + pad4(namesize - 2) + pad4(filesize)]);
    if ((mode & 0xF000) == 0x4000) {
      vfs_mkdir(&(cur->data[relate_st]), dir);
      vfs_chdir(&(cur->data[relate_st]), dir);
      // puts("dir\n");
      // puts("in\n");
      int rv = _parse_initramfs(dir, cur_pos, cur);
      if (rv == 1) {
        vfs_chdir("..", dir);
        return 1;
      } else if (rv == 0) {
        vfs_chdir("..", dir);
      } else {
        return -1;
      }

    } else if ((mode & 0xF000) == 0x8000) {
      // puts("file\n");
      struct file fd;
      vfs_open(&(cur->data[relate_st]), dir, &fd);
      vfs_write(&fd, &cur->data[2 + pad4(namesize - 2)], filesize);
      vfs_close(&fd);
    } else {
      return -1;
    }
  }
  return -1;
}

void parse_initramfs(struct dentry *root) {
  puts("Parse initramfs.cpio\n");
  struct cpio_newc_header *cpio_itr = (struct cpio_newc_header *)INITRAMFS;
  if (strcmp_n(cpio_itr->c_magic, "070701", 6)) {
    puts("initramfs.cpio error\n");
    return;
  }
  cpio_itr->data[0] = 0;
  struct cpio_newc_header *cur_pos =
      (struct cpio_newc_header *)&(cpio_itr->data[2]);
  int rv = _parse_initramfs(root, &cur_pos, cpio_itr);
  if (rv != 1) {
    puts("parse error\n");
  }

  struct ramfs_inode *dd = (struct ramfs_inode *)(root->node->internal);
  struct ramfs_child_list *list = (struct ramfs_child_list *)(dd->data);
  for (int i = 0; i < dd->size; i++) {
    puts(list->child[i % MAX_DIR]->name);
    putc('\n');
    if (i % MAX_DIR == MAX_DIR - 1) {
      list = list->next;
    }
  }
  struct file f;
  vfs_open("include/mem.h", root, &f);
  puts_n(((struct ramfs_inode *)(f.file_node->internal))->data, 50);
}