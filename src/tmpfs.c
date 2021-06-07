#include "tmpfs.h"

#include "exc.h"
#include "io.h"
#include "mem.h"
#include "sched.h"

typedef struct page_block page_block;

struct page_block {
  page_block *next;
  void *page;
};

typedef struct tmpfs_node {
  char *name;
  unsigned long size;
  unsigned long mode;
  unsigned long pb_cnt;
  page_block *pb_head;
} tmpfs_node;

inode_op *tmpfs_i_op;
dentry_op *tmpfs_d_op;
file_op *tmpfs_f_op;
file_system_t *tmpfs_t;

tmpfs_node *new_tmpfs_node(const char *name, unsigned long mode) {
  tmpfs_node *tn = kmalloc(sizeof(tmpfs_node));
  tn->name = new_str(name);
  tn->size = 0;
  tn->mode = mode;
  tn->pb_cnt = 0;
  tn->pb_head = NULL;
  return tn;
}

int tmpfs_mount(file_system_t *fs, dentry *mp, const char *dev) {
  log("mnt ", LOG_PRINT);
  log(dev, LOG_PRINT);
  log("\n", LOG_PRINT);
  if (strcmp(dev, "tmpfs") != 0) {
    return INVALID_DEV;
  }
  super_block *new_sb = kmalloc(sizeof(super_block));
  new_sb->mnt_p = mp;
  inode *root_node = kmalloc(sizeof(inode));
  dentry *dent = kmalloc(sizeof(dentry));
  dent->name = "";
  dent->d_op = tmpfs_d_op;
  dent->d_inode = root_node;
  dent->parent = NULL;
  init_cdl_list(&(dent->child));
  init_cdl_list(&(dent->sibli));
  dent->mnt = NULL;
  dent->d_data = NULL;
  dent->ref_cnt = 1;
  new_sb->root = dent;
  root_node->i_sb = new_sb;
  root_node->i_op = tmpfs_i_op;
  root_node->i_mode = TYPE_DIR;
  root_node->i_size = 0;
  tmpfs_node *tn = new_tmpfs_node("", TYPE_DIR);
  root_node->i_data = tn;
  push_cdl_list(&(fs->sb_list), &(new_sb->sb_list));
  mp->mnt = new_sb;
  mp->ref_cnt++;
  return 0;
}

tmpfs_node **tmpfs_find(tmpfs_node *node, const char *name) {
  if (node->size == 0) {
    return NULL;
  }
  page_block *pb_itr = node->pb_head;
  unsigned long entry_size = node->size / sizeof(tmpfs_node *);
  tmpfs_node **node_itr = pb_itr->page;
  for (unsigned long i = 0; i < entry_size; i++) {
    if (i != 0 && i % (PAGE_SIZE / sizeof(tmpfs_node *)) == 0) {
      pb_itr = pb_itr->next;
      node_itr = pb_itr->page;
    }
    if (strcmp((*node_itr)->name, name) == 0) {
      return node_itr;
    }
    node_itr++;
  }
  return NULL;
}

void write_node(tmpfs_node *node, unsigned long offset, const char *buf,
                unsigned long len) {
  unsigned long st_pg_offset = offset / PAGE_SIZE;
  unsigned long require_pg_cnt = (offset + len + PAGE_SIZE - 1) / PAGE_SIZE;
  if (node->pb_cnt < require_pg_cnt) {
    page_block **pb_itr = &(node->pb_head);
    while ((*pb_itr) != NULL) {
      pb_itr = &((*pb_itr)->next);
    }
    while (node->pb_cnt < require_pg_cnt) {
      *pb_itr = kmalloc(sizeof(page_block));
      (*pb_itr)->page = kmalloc(PAGE_SIZE);
      (*pb_itr)->next = NULL;
      node->pb_cnt++;
      pb_itr = &((*pb_itr)->next);
    }
  }
  page_block *pb_itr = node->pb_head;
  for (unsigned long i = 0; i < st_pg_offset; i++) {
    pb_itr = pb_itr->next;
  }
  char *buf_itr = pb_itr->page;
  buf_itr += (offset % PAGE_SIZE);
  unsigned long write_pos = offset;
  for (unsigned long i = 0; i < len; i++) {
    if (i != 0 && write_pos % PAGE_SIZE == 0) {
      pb_itr = pb_itr->next;
      buf_itr = pb_itr->page;
    }
    *buf_itr++ = *buf++;
    write_pos++;
  }
  node->size = write_pos;
}

unsigned long read_node(tmpfs_node *node, unsigned long offset, char *buf,
                        unsigned long len) {                     
  // log("?", LOG_PRINT);
  asm volatile ("":::"memory");   
  if (offset >= node->size) {
    return 0;
  }
  unsigned long st_pg_offset = offset / PAGE_SIZE;

  page_block *pb_itr = node->pb_head;
  for (unsigned long i = 0; i < st_pg_offset; i++) {
    pb_itr = pb_itr->next;
  }
  char *buf_itr = pb_itr->page;
  buf_itr += (offset % PAGE_SIZE);

  unsigned long read_pos = offset;
  for (unsigned long i = 0; i < len && read_pos < node->size; i++) {
    if (i != 0 && read_pos % PAGE_SIZE == 0) {
      pb_itr = pb_itr->next;
      buf_itr = pb_itr->page;
    }
    *buf++ = *buf_itr++;
    read_pos++;
    log("+", LOG_PRINT);
  }
  return read_pos - offset;
}

void shrink_node(tmpfs_node *node) {}

void new_entry(tmpfs_node *node, const char *name, unsigned long mode) {
  tmpfs_node *tn = new_tmpfs_node(name, mode);
  write_node(node, node->size, (char *)(&tn), sizeof(tmpfs_node *));
}

int tmpfs_create(dentry *dent, const char *name) {
  if (dent->d_inode->i_mode != TYPE_DIR) {
    return INVALID_PATH;
  }
  disable_interrupt();
  if (tmpfs_find((tmpfs_node *)(dent->d_inode->i_data), name) != NULL) {
    enable_interrupt();
    return TARGET_EXIST;
  }
  new_entry((tmpfs_node *)(dent->d_inode->i_data), name, TYPE_FILE);
  dent->d_inode->i_size = ((tmpfs_node *)(dent->d_inode->i_data))->size;
  enable_interrupt();
  log("crt\n", LOG_PRINT);
  return 0;
}

// void del_entry(tmpfs) {

// }

int tmpfs_remove(dentry *dent, const char *name) {
  if (dent->d_inode->i_mode != TYPE_DIR) {
    return INVALID_PATH;
  }
  disable_interrupt();
  tmpfs_node **node_pos =
      tmpfs_find((tmpfs_node *)(dent->d_inode->i_data), name);
  if (node_pos == NULL) {
    enable_interrupt();
    return TARGET_NO_EXIST;
  }
  return 0;
}

int tmpfs_mkdir(dentry *dent, const char *name) {
  if (dent->d_inode->i_mode != TYPE_DIR) {
    return INVALID_PATH;
  }
  disable_interrupt();
  if (tmpfs_find((tmpfs_node *)(dent->d_inode->i_data), name) != NULL) {
    enable_interrupt();
    return TARGET_EXIST;
  }
  new_entry((tmpfs_node *)(dent->d_inode->i_data), name, TYPE_DIR);
  dent->d_inode->i_size = ((tmpfs_node *)(dent->d_inode->i_data))->size;
  enable_interrupt();
  return 0;
}

int tmpfs_rmdir(dentry *dent, const char *name) { return 0; }

int tmpfs_open(dentry *dent, file **f, unsigned long flag) {
  log_hex("opn ", dent->ref_cnt, LOG_PRINT);
  file *new_f = kmalloc(sizeof(file));
  new_f->path = dent;
  dent->ref_cnt++;
  new_f->f_op = tmpfs_f_op;
  new_f->f_inode = dent->d_inode;
  new_f->f_mode = flag;
  new_f->f_pos = 0;
  *f = new_f;
  log_hex(dent->name, dent->d_inode->i_size, LOG_PRINT);
  return 0;
}

int tmpfs_close(struct file *f) {
  vfs_closedent(f->path);
  kfree(f);
  return 0;
}

int tmpfs_opendent(dentry *dent, dentry **target, const char *name) {
  tmpfs_node *tn = dent->d_inode->i_data;
  if (tn->mode != TYPE_DIR) {
    return INVALID_PATH;
  }
  disable_interrupt();
  tmpfs_node **new_node = tmpfs_find(tn, name);
  if (new_node == NULL) {
    enable_interrupt();
    return TARGET_NO_EXIST;
  }
  dentry *new_dent = kmalloc(sizeof(dentry));
  inode *new_inode = kmalloc(sizeof(inode));
  new_inode->i_sb = dent->d_inode->i_sb;
  new_inode->i_op = tmpfs_i_op;
  new_inode->i_mode = (*new_node)->mode;
  new_inode->i_size = (*new_node)->size;
  new_inode->i_data = (*new_node);
  new_dent->name = (*new_node)->name;
  new_dent->d_op = tmpfs_d_op;
  new_dent->d_inode = new_inode;
  new_dent->parent = dent;
  dent->ref_cnt++;
  init_cdl_list(&(new_dent->child));
  push_cdl_list(&(dent->child), &(new_dent->sibli));
  new_dent->mnt = NULL;
  new_dent->d_data = NULL;
  new_dent->ref_cnt = 1;
  *target = new_dent;
  enable_interrupt();
  log("opd\n", LOG_PRINT);
  return 0;
}

int tmpfs_closedent(dentry *dent) {
  dentry *parent = dent->parent;
  disable_interrupt();
  pop_cdl_list(&(dent->sibli));
  kfree(dent->d_inode);
  kfree(dent);
  vfs_closedent(parent);
  enable_interrupt();
  return 0;
}

int tmpfs_getdent(dentry *dent, unsigned long offset, struct dirent *d) {
  return 0;
}

long tmpfs_read(struct file *f, char *buf, unsigned long len) {
  long rl = read_node(f->f_inode->i_data, f->f_pos, buf, len);
  f->f_pos += rl;
  return rl;
}
long tmpfs_write(struct file *f, const char *buf, unsigned long len) {
  write_node(f->f_inode->i_data, f->f_pos, buf, len);
  f->f_pos += len;
  return len;
}

int tmpfs_umount(dentry *dent) { return 0; }

void setup_tmpfs() {
  tmpfs_t = kmalloc(sizeof(file_system_t));
  tmpfs_t->name = "tmpfs";
  init_cdl_list(&(tmpfs_t->sb_list));
  tmpfs_t->mount = &tmpfs_mount;
  tmpfs_i_op = kmalloc(sizeof(inode_op));
  tmpfs_i_op->create = &tmpfs_create;
  tmpfs_i_op->remove = &tmpfs_remove;
  tmpfs_i_op->mkdir = &tmpfs_mkdir;
  tmpfs_i_op->rmdir = &tmpfs_rmdir;
  tmpfs_i_op->open = &tmpfs_open;
  tmpfs_i_op->close = &tmpfs_close;
  tmpfs_i_op->opendent = &tmpfs_opendent;
  tmpfs_i_op->closedent = &tmpfs_closedent;
  tmpfs_i_op->getdent = &tmpfs_getdent;
  tmpfs_d_op = kmalloc(sizeof(dentry_op));
  tmpfs_d_op->umount = &tmpfs_umount;
  tmpfs_f_op = kmalloc(sizeof(file_op));
  tmpfs_f_op->read = &tmpfs_read;
  tmpfs_f_op->write = &tmpfs_write;
  register_fs(tmpfs_t);
}