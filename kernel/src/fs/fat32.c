#include "fat32.h"
#include "vfs.h"
#include "sdhost.h"
#include "io.h"

struct filesystem fat32 = {
    .name = "fat32",
    .setup_mount = fat32_setup
};

struct vnode_operations fat32_v_ops = {
    .lookup = fat32_lookup,
    .create = fat32_create
};

struct file_operations fat32_f_ops = {
    .write = fat32_write,
    .read = fat32_read
};

int fat32_setup(struct filesystem *fs, struct mount* mount)
{
    // migrate root from previous filesystem to tmpfs
    mount->fs = fs;
    mount->root->mount = mount;
    mount->root->v_ops = &fat32_v_ops;
    mount->root->f_ops = &fat32_f_ops;

    // create root internal
    // mount->root->internal = create_tmpfs_vnode_internal("/");
    // cpio_init_fs(mount->root);

    // printf("12\n");
    char buffer[512];

    // first partition start from 2048
    // read Boot Sector 
    readblock(2048, buffer);
    printf("FAT32 information: \n");
    printf("Bytes per logical sector: %d\n", *((short *)&buffer[0x00B]));
    printf("Sectors per cluster: %d\n", buffer[0x00D]);
    printf("Number of File Allocation Tables: %d\n", buffer[0x010]);
    printf("Root directory start cluster: %d\n", *((int *)&buffer[0x02C]));
    

    



    return 0;
}

int fat32_lookup(struct vnode* dir_node, struct vnode **target, const char* component_name)
{
    return 0;
}

int fat32_create(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    return 0;
}

int fat32_write (struct file *file, const void *buf, size_t len)
{
    return 0;
}

int fat32_read (struct file *file, void *buf, size_t len)
{
    return 0;
}