#ifndef _FAT32_H_
#define  _FAT32_H_

#include "vfs.h"

int fat_set_mount(mount_t*, const char*);
void fat_init(void);

#endif
