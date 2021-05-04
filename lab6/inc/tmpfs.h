#include "vfs.h"

int tmpfs_Setup(filesystem* fs,mount* mnt);//setup vnode
void tmpfs_fopsGet(file_operations* fops);//setup file