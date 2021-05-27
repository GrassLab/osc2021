#include "fat32.h"
#include "vfs.h"
#include "sdhost.h"
#include "io.h"
#include "string.h"
#include "dynamic_alloc.h"

int PARTITION_BASE_BLOCK_INDEX = 2048,
    FAT_BASE_BLOCK_INDEX,
    DATA_BASE_BLOCK_INDEX,
    ROOT_CLUSTER_NUMBER;

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
    // migrate root from previous filesystem to fat32
    mount->fs = fs;

    // build root
    mount->root->mount = mount;
    mount->root->v_ops = &fat32_v_ops;
    mount->root->f_ops = &fat32_f_ops;

    // create root internal
    struct file_info f_info = {
        .name = "/",
        .size = 0
    };

    // prepare entry indo
    struct entry_info e_info = {
        .start_cluster = 0,
        .dir_cluster = 0,
        .entry_index = 0
    };
    mount->root->internal = create_fat32_vnode_internal(&f_info, &e_info);


    // display information
    int bytes_per_logical_sector, 
        sectors_per_cluster, 
        n_fat_tables, 
        root_start_cluster, 
        sectors_per_fat, 
        n_reserved_sectors;

    char buffer[512];
    // first partition start from 2048
    
    readblock(PARTITION_BASE_BLOCK_INDEX, buffer);

    // read Boot Sector 
    printf("FAT32 information: \n");
    printf("Bytes per logical sector: %d\n", (bytes_per_logical_sector = *((short *)&buffer[0x00B])));
    printf("Sectors per cluster: %d\n", (sectors_per_cluster = buffer[0x00D]));
    printf("Number of File Allocation Tables: %d\n", (n_fat_tables = buffer[0x010]));
    printf("Root directory start cluster: %d\n", (root_start_cluster = *((int *)&buffer[0x02C])));
    printf("Logical sectors per file allocation table: %d\n", (sectors_per_fat = *((int *)&buffer[0x024])));
    printf("Count of reserved logical sectors: %d\n\n", (n_reserved_sectors = *((short *)&buffer[0x00E])));
    ROOT_CLUSTER_NUMBER = root_start_cluster;
    FAT_BASE_BLOCK_INDEX = PARTITION_BASE_BLOCK_INDEX + n_reserved_sectors;
    DATA_BASE_BLOCK_INDEX = FAT_BASE_BLOCK_INDEX + n_fat_tables * sectors_per_fat;
    printf("data region start index: %d\n\n", DATA_BASE_BLOCK_INDEX);

    sd_init_fs(mount->root);
    
    
    return 0;
}

static void append_child(struct vnode *parent, struct vnode *node)
{
    struct fat32_internal *parent_internal = parent->internal;
    if (!parent_internal->l_child) {
        parent_internal->l_child = node;
    } else {
        struct vnode *tmp_node = parent_internal->l_child;
        struct fat32_internal *tmp_internal = tmp_node->internal;
        while (tmp_internal->r_sibling) {
            tmp_node = tmp_internal->r_sibling;
            tmp_internal = tmp_node->internal;
        }

        tmp_internal->r_sibling = node;
    }
}

int fat32_lookup(struct vnode* dir_node, struct vnode **target, const char* component_name)
{
    struct fat32_internal *internal = dir_node->internal;

    // terminate condition
    if (strcmp(internal->f_info.name, component_name) == 0) {
        *target = dir_node;
        printf("found!\n");
        return 0;
    }

    // if it's root, find child
    if (internal->f_info.name[0] == '/' && internal->l_child) {
        if (component_name[0] == '/') {
            component_name++; // shift out first character
        }
        
        return fat32_lookup(internal->l_child, target, component_name);
    }

    // get first fragment
    char frag[100] = { 0 };
    int offset = get_first_frag(frag, component_name);
    component_name += offset;

    struct vnode *tmp_vnode = dir_node;
    
    // search each siblings
    while (((struct fat32_internal *)tmp_vnode->internal)->r_sibling) {
        struct vnode *r_sibling = ((struct fat32_internal *)tmp_vnode->internal)->r_sibling;
        struct fat32_internal *r_sibling_internal = r_sibling->internal;

        if (strcmp(r_sibling_internal->f_info.name, frag) == 0) {
            // sibling match
            if (strlen(component_name) == 0) {
                *target = r_sibling;
                return 0;
            }

            // lookup its child
            return fat32_lookup(r_sibling_internal->l_child, target, component_name);
        }

        // not this sibling, try next
        tmp_vnode = r_sibling;
    }

    printf("not found!\n");
    // finally not found
    return -1;
}

int fat32_create(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    return 0;
}

int fat32_write (struct file *file, const void *buf, size_t len)
{
    struct fat32_internal *internal = file->vnode->internal;
    int start_cluster = internal->e_info.start_cluster;
    int target_block = DATA_BASE_BLOCK_INDEX + start_cluster + (file->f_pos / 512),
        offset = file->f_pos % 512;

    char _buffer[512];
    readblock(target_block, _buffer);
    for (int i = 0; i < len; i++) {
        _buffer[offset + i] = ((char *)buf)[i];
    }

    writeblock(target_block, _buffer);

    file->f_pos += len;


    internal->f_info.size = internal->f_info.size > file->f_pos ? internal->f_info.size : file->f_pos;
    fat32_update_size(file->vnode, internal->f_info.size);

    return len;
}

int fat32_read (struct file *file, void *buf, size_t len)
{
    struct fat32_internal *internal = file->vnode->internal;
    int start_cluster = internal->e_info.start_cluster;
    // int filesize = internal->size;

    int target_block = DATA_BASE_BLOCK_INDEX + start_cluster + (file->f_pos / 512),
        offset = file->f_pos % 512;

    char _buffer[512];
    readblock(target_block, _buffer);

    strncpy(buf, &_buffer[offset], len);
    
    file->f_pos += len;

    return len;
}

void sd_init_fs(struct vnode *root)
{
    char buffer[512];

    // root directory is at the begining of data region's head
    readblock(DATA_BASE_BLOCK_INDEX, buffer);

    // loop all root directory entries
    int i = 0;
    int j = 0; // entry index
    while (buffer[i] != 0 && buffer[i] != 0xE5) {
        // first 8 bytes are filename
        int filesize = 0;
        char filename[10] = { 0 }, ext[4] = { 0 };
        int k = 0;
        while (buffer[i + k] != ' ' && k < 8) {
            filename[k] = buffer[i + k];
            k++;
        }

        // 3 bytes are ext name
        strncpy(ext, &buffer[i + 0x08], 3);

        filesize = *((int *)&buffer[i + 0x1C]);

        int cluster_index;
        cluster_index = *(short *)&buffer[i + 0x14];
        cluster_index = (cluster_index << 16) + *(short *)&buffer[i + 0x1A];
        cluster_index -= ROOT_CLUSTER_NUMBER;

        printf("filename is %s.%s\n", filename, ext);
        printf("file size: %d\n", filesize);

        // prepare file info
        struct file_info f_info;
        copy_filename_with_ext(f_info.name, filename, ext);
        f_info.size = filesize;
         

        // prepare entry indo
        struct entry_info e_info;
        e_info.start_cluster = cluster_index;
        e_info.dir_cluster = 0;
        e_info.entry_index = j;

        struct vnode *node = create_fat32_vnode(root, &f_info, &e_info);

        append_child(root, node);

        i += 32;
        j++;
    }
}

struct vnode *create_fat32_vnode(struct vnode *parent, struct file_info *f_info, struct entry_info *e_info)
{
    struct vnode *vnode = malloc(sizeof(struct vnode));
    vnode->f_ops = parent->f_ops;
    vnode->mount = parent->mount;
    vnode->v_ops = parent->v_ops;


    vnode->internal = create_fat32_vnode_internal(f_info, e_info);

    return vnode;
}

struct fat32_internal *create_fat32_vnode_internal(struct file_info *f_info, struct entry_info *e_info)
{
    struct fat32_internal *internal = malloc(sizeof(struct fat32_internal));
    strcpy(internal->f_info.name, f_info->name);
    internal->f_info.size = f_info->size;

    internal->e_info.dir_cluster = e_info->dir_cluster;
    internal->e_info.entry_index = e_info->entry_index;
    internal->e_info.start_cluster = e_info->start_cluster;

    return internal;
}


void fat32_update_size(struct vnode *node, int size)
{
    struct fat32_internal *internal = node->internal;
    char buffer[512];
    readblock(DATA_BASE_BLOCK_INDEX + internal->e_info.dir_cluster, buffer);
    
    int *size_ptr = (int *)&(buffer[32 * internal->e_info.entry_index + 0x1C]);
    *size_ptr = size;

    writeblock(DATA_BASE_BLOCK_INDEX + internal->e_info.dir_cluster, buffer);
}

// void parse_name(const char *component_name, char *name, char *ext)
// {
//     int i;
//     for (i = 0; i < strlen(component_name); i++) {
//         if (component_name[i] == '.') {
//             printf("break!\n");
//             break;
//         }

//         name[i] = component_name[i];
//     }


//     if (component_name[i] == '.') {
//         int j;
//         for (j = 0; j < 3; j++) {
//             ext[j] = component_name[i + j + 1];
//         }
//     }
// }

void copy_filename_with_ext(char *target, char *name, char *ext)
{
    int i = 0;
    while ( name[i] != '\0' && name[i] != ' ' ) {
        target[i] = name[i];
        i++;
    }

    if (strlen(ext) > 0) {
        target[i] = '.';

        int j;
        for (j = 0; j < 3; j++) {
            target[i + j + 1] = ext[j];
        }
        target[i + j + 1] = '\0';
    }
}