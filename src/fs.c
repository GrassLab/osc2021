#include "vfs.h"
#include "tmpfs.h"

struct filesystem tmpfs = {
    .name = "tmpfs",
    .setup_mount = tmpfs_setup_mount
};