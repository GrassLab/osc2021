#pragma once

#include "fs/vfs.h"

int fat_setup_mount(struct filesystem *fs, struct mount *mount);
void fat_dev();
extern struct filesystem fat;
