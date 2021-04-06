// #include "vfs.h"

// #include "io.h"
// #include "mem.h"
// #include "util.h"

// superblock *rootfs;

// void init_rootfs(superblock *sb) { rootfs = sb; }

// int mkdir(const char *path, dentry *dir) {}
// int rmdir(const char *path, dentry *dir) {}
// // int rndir(const char *name, dentry *dir) {}
// int opendir(const char *path, dentry *dir) {}
// int closedir(dentry *dir) {}
// int chdir(const char *path, dentry *dir) {}
// // int listdir(dentry *dir, int *size) {}

// int open(const char *path, dentry *dir, file *f, unsigned long flag) {}
// int read(file *f, char *buf, size_t len) {}
// int write(file *f, const char *buf, size_t len) {}
// int close(file *f) {}
// // int rnfile(const char *name, file *f) {}

// void init_dentry(dentry *dir) { dir->node = NULL; }
// void init_file(file *f) { f->node = NULL; }
