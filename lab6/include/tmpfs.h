#ifndef _TMPFS_H
#define _TMPFS_H
#include "vfs.h"
int tmpfsSetup(filesystem* fs, mount* mnt);
void tmpfsfopsGet(file_operations* f_ops);

#endif
