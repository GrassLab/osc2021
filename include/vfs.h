// #ifndef VFS_H
// #define VFS_H

// #include <stddef.h>

// typedef struct inode {
//   struct superblock *sb;
//   unsigned long ref_cnt;
//   void *inter;
// } inode;

// typedef struct dentry {
//   struct inode *node;
// } dentry;

// typedef struct file {
//   struct inode *node;
// } file;

// typedef struct superblock_ops {
//   int (*mkdir)(const char *path, dentry *dir);
//   int (*chdir)(const char *path, dentry *dir);
//   int (*rmdir)(const char *path, dentry *dir);
//   int (*rndir)(const char *name, dentry *dir);
//   int (*opendir)(const char *path, dentry *dir, superblock *sb);
//   int (*closedir)(dentry *dir);
//   int (*listdir)(dentry *dir, int *size);

//   int (*open)(const char *path, dentry *dir, file *f, unsigned long flag);
//   int (*read)(file *f, char *buf, size_t len);
//   int (*write)(file *f, const char *buf, size_t len);
//   int (*close)(file *f);
//   int (*rnfile)(const char *name, file *f);

// } superblock_ops;

// typedef struct superblock {
//   int mount_id;
//   struct superblock *parent;
//   const char *name;
//   struct mount_list *mnt_l;
//   struct superblock_ops sb_ops;
//   void *inter;
// } superblock;

// typedef struct vfsmount {
//   int mount_id;
//   struct superblock *sb;
//   char *path;
// };

// struct mount_list {
//   struct vfsmount mount;
//   struct mount_list *next;
// };

// int mkdir(const char *path, dentry *dir);
// int rmdir(const char *path, dentry *dir);
// // int rndir(const char *name, dentry *dir);
// int opendir(const char *path, dentry *dir);
// int closedir(dentry *dir);
// int chdir(const char *path, dentry *dir);
// // int listdir(dentry *dir, int *size);

// int open(const char *path, dentry *dir, file *f, unsigned long flag);
// int read(file *f, char *buf, size_t len);
// int write(file *f, const char *buf, size_t len);
// int close(file *f);
// // int rnfile(const char *name, file *f);

// void init_rootfs(superblock *sb);
// void init_dentry(dentry *dir);
// void init_file(file *f);

// #endif