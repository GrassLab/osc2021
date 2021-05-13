#ifndef TMPFS_H
#define TMPFS_H

#define DIR_TYPE 1
#define FILE_TYPE 2
#define DIR_CAP 10

typedef struct internal_content {
    char *name;
    unsigned long type;
    unsigned long capacity;
    unsigned long size; // number of childern for a directory
    void *data; // directory: collect vnode of all childern; file: data of file
} iContent;

int tmpfs_setup(filesystem *fs, mount *mnt);
int tmpfs_read(file* file, void* buf, unsigned long len);
int tmpfs_write(file* file, const void* buf, unsigned long len);
int tmpfs_lookup(vnode* dir_node, vnode** target, const char* component_name);

#endif