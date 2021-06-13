#ifndef _TMPFS_H_
#define  _TMPFS_H_

#include "vfs.h"

#define MAX_FILE_SIZE 2048

void tmpfs_init();
void tmpfs_setup_mount(mount_t*, const char*);

#endif
