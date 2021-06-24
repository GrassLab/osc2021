#ifndef VFS_H
#define VFS_H

#include <stddef.h>

#include "util.h"

// super block for a file system
typedef struct super_block {
  cdl_list sb_list;      // sb list of fs
  struct dentry *root;   // root dentry of this sb
  struct dentry *mnt_p;  // dentry this sb mount on
} super_block;

// unique
typedef struct inode {
  struct super_block *i_sb;
  struct inode_op *i_op;
  unsigned long i_mode;
  unsigned long i_size;
  void *i_data;
} inode;

// unique except hard link
typedef struct dentry {
  char *name;  // component name
  struct dentry_op *d_op;
  inode *d_inode;
  struct dentry *parent;
  cdl_list child;
  cdl_list sibli;
  struct super_block *mnt;  // mount point
  void *d_data;
  unsigned long ref_cnt;
} dentry;

typedef struct file_system_t file_system_t;

// file system type
struct file_system_t {
  cdl_list fs_list;
  const char *name;
  int (*mount)(file_system_t *, dentry *, const char *);
  cdl_list sb_list;
};

typedef struct file {
  struct dentry *path;
  struct file_op *f_op;
  inode *f_inode;
  unsigned long f_mode;  // open mode
  unsigned long f_pos;   // read / write pos
} file;

typedef struct dirent {
  char name[512];
  unsigned long size;
  unsigned long mode;
} dirent;

// open* and close* is required
typedef struct inode_op {
  // char * is component name possible null string
  int (*create)(struct dentry *, const char *);
  // char * is component name possible null string
  int (*remove)(struct dentry *, const char *);
  // char * is component name possible null string
  int (*mkdir)(struct dentry *, const char *);
  // char * is component name possible null string
  int (*rmdir)(struct dentry *, const char *);
  // dentry is the file to be open
  int (*open)(struct dentry *, struct file **, unsigned long);
  int (*close)(struct file *);
  // char * is component name dentry * possible file node
  int (*opendent)(struct dentry *, struct dentry **, const char *);
  int (*closedent)(struct dentry *);
  int (*getdent)(struct dentry *, unsigned long, struct dirent *);
} inode_op;

typedef struct dentry_op {
  int (*umount)(dentry *);
} dentry_op;

typedef struct file_op {
  long (*read)(struct file *, char *, unsigned long);
  long (*write)(struct file *, const char *, unsigned long);
} file_op;

int vfs_create(const char *path);
int vfs_remove(const char *path);
int vfs_mkdir(const char *path);
int vfs_rmdir(const char *path);
unsigned long vfs_open(const char *path, unsigned long flag);
int vfs_close(unsigned long fd_num);
int vfs_opendent(struct dentry **target, const char *path);
int vfs_closedent(struct dentry *target);
int vfs_getdent(unsigned long fd_num, unsigned long count,
                struct dirent *dent);
long vfs_read(unsigned long fd_num, char *buf, unsigned long size);
long vfs_write(unsigned long fd_num, const char *buf, unsigned long size);

typedef struct file_discriptor {
  cdl_list fd_list;
  unsigned long fd_num;
  file *f;
} file_discriptor;

#define METHOD_NOT_IMP -1
#define INVALID_PATH -2
#define INVALID_FD -3
#define INVALID_FS -4
#define INVALID_DEV -5
#define TARGET_EXIST -6
#define TARGET_NO_EXIST -7
#define TARGET_INUSE -8

#define TYPE_DIR 1
#define TYPE_FILE 2

#define get_struct_head(stc, com, addr) \
  ((stc *)((void *)addr - offsetof(stc, com)))

void init_vfs();

int vfs_mount(const char *dev, const char *mp, const char *fs);
int vfs_umount(const char *path);
int vfs_chdir(const char *path);
dentry *get_vfs_root();
void register_fs(file_system_t *fs);

#define O_CREATE 1 << 0

unsigned long get_filesize(unsigned long fd_num);

#endif