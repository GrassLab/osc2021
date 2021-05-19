#include "../include/vfs.h"
#include "../include/initrd.h"
#include "../include/stringUtils.h"
#include "../include/memAlloc.h"
#include "../include/tmpfs.h"


static filesystem my_filesystem;
static mount my_mount;

int vfs_init(void *setup_mount_f, void *write_f,void *read_f){
    int (*setup_mount)(filesystem* fs,mount* mnt) = setup_mount_f;

    setup_mount(&my_filesystem,&my_mount);
    uart_printf("%s fs has been setup\n",my_filesystem.name);
}

