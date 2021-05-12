#ifndef	_VFS_H
#define	_VFS_H

#include "types.h"
#include "list.h"

#define REGULAR_FILE 0
#define DIRECTORY    1

#define O_CREAT 1

#define NR_OPEN_DEFAULT 32

#define DNAME_INLINE_LEN 64 // max dentry size

#define VFS_ERROR -1
#define VFS_CREATE_FILE_ERROR -2

#define VFS_OPEN_FILE_NOT_VALID_PATH_ERROR -2
#define VFS_OPEN_FILE_ERROR -1
#define VFS_READ_FILE_ERROR -1
#define VFS_WRITE_FILE_ERROR -1
/**
 * VFS node
 * 
 * vnode is called inode in linux
 */
struct vnode
{
    struct dentry *dentry;
    struct vnode_operations* v_ops;
    struct file_operations* f_ops;
    int v_type;
    void *internal; // internal representation of a vnode 
};

struct dentry
{
    char name[DNAME_INLINE_LEN];
    struct dentry *parent;     // parent directory
    struct vnode *vnode; 

    struct list_head list;     // child of parent list 
    struct list_head sub_dirs; // our children
};

struct file 
{
    struct vnode *vnode;
    size_t f_pos; // The next read/write position of this opened file
    struct file_operations* f_ops;
    int flags; // unused
    int nr_internal_moemory_page_allocated;
};

struct mount 
{
    struct dentry *root; // root directory
    struct filesystem *fs;     
};

struct filesystem
{
    const char *name;
    int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct vnode_operations
{
    int (*lookup)(struct vnode *dir_node, struct vnode **target, const char *component_name);
    int (*create)(struct vnode *dir_node, struct vnode **target, const char *component_name);
};

struct file_operations
{
    int (*write)(struct file *file, const void *buf, size_t len);
    int (*read)(struct file *file, void *buf, size_t len);
};

/**
 * file descriptor table for each process
 */
struct files_struct
{
    int count;
    int next_fd;
    // struct file **fd;
    struct file * fd_array[NR_OPEN_DEFAULT]; // array of file descriptor
};


void rootfs_init();

int _vnode_path_traversal(struct vnode *rootnode, const char* pathname, struct vnode **target_file, char *target_component_name);
int _lookUp_pathname(const char* pathname, struct vnode **target_file, char *target_component_name);
/* VFS API */
int register_filesystem(struct filesystem* fs);

struct file *vfs_open(const char *pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file *file, const void *buf, size_t len);
int vfs_read(struct file *file, void *buf, size_t len);

char *vfs_read_directory(struct file *file);
void vfs_print_directory_by_pathname(const char *pathname);
void vfs_populate_initramfs();

extern struct mount *rootfs;

void _vfs_dump_vnode();
void _vfs_dump_dentry();
void _vfs_dump_file_struct();

/* Test case for vfs */
void vfs_test();
void vfs_requirement1_test();
void vfs_requirement1_read_file_populated_in_cpio();
void vfs_user_process_test();
void vfs_ls_print_test();
#endif