# include "vfs.h"

# define TMPFS_DEFAULT_MODE 6

void tmpfs_init();
int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount);
