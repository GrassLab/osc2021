#include "ramfs.h"

#include "io.h"
#include "mem.h"
#include "util.h"

#define pad4(x) ((((x) + 3) / 4) * 4)

int ramfs_mkdir(const char *path, struct dentry *dir) {
  puts("mkdir ");
  puts(path);
  putc('\n');
  return 0;
}
int ramfs_chdir(const char *path, struct dentry *dir) {
  puts("chdir ");
  puts(path);
  putc('\n');
  return 0;
}

int ramfs_rmdir(const char *path, struct dentry *dir) { return 0; }

int ramfs_open(const char *path, struct dentry *dir, struct file *f) {
  f->dir_node = dir->node;
  f->file_node = dir->node;
  puts("open ");
  puts(path);
  putc('\n');
  return 0;
}

int ramfs_read(struct file *f, char *buf, size_t len) { return 0; }

int ramfs_write(struct file *f, const char *buf, size_t len) {
  puts("write ");
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
  dir->node = kmalloc(sizeof(struct inode));
  dir->node->sb = sb;
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
}