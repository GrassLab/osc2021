#ifndef __FS_FS_STRUCT_H_
#define __FS_FS_STRUCT_H_
#include <fs/vfs.h>

struct path {
	struct mount *mnt;
	struct vnode *node;
};

struct fs_struct {
	struct path root, pwd;
};

#endif