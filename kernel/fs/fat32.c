#include "fat32.h"
#include "sd.h"
#include "../../lib/uart.h"
#include "../../lib/string.h"

#define BASE_PARTITION_BLOCK_SIZE 2048

int FAT_BASE_BLOCK;
int DATA_BASE_BLOCK;
int ROOT_CLUSTER;

struct filesystem fatfs = {
    .name = "fat32",
    .setup_mount = fat32_setup
};

struct file_operations fat_f_ops = {
    .write = fat32_write,
    .read = fat32_read
};

struct vnode_operations fat_v_ops = {
    .lookup = fat32_lookup,
    .create = fat32_create
};

int fat32_setup(struct filesystem * fs, struct mount * mount)
{
    mount->fs = fs;
    mount->root->mount = mount;
    mount->root->v_ops = &fat_v_ops;
    mount->root->f_ops = &fat_f_ops;
    mount->root->internel = fat32_create_internel("/", 0, 0, 0);

    int bytes_per_logical_sector,
        sectors_per_cluster,
        n_fat_tables,
        root_start_cluster,
        sectors_per_fat,
        n_reserved_sectors;

    char buffer[FAT_BLOCK_SIZE];

    readblock(BASE_PARTITION_BLOCK_SIZE, buffer);

    bytes_per_logical_sector = *((short*)&buffer[0x00B]);
    sectors_per_cluster = buffer[0x00D];
    n_fat_tables = buffer[0x010];
    root_start_cluster = *((int*)buffer[0x02C]);
    sectors_per_fat = *((int*)buffer[0x024]);
    n_reserved_sectors = *((short*)buffer[0x00E]);
    
    ROOT_CLUSTER = root_start_cluster;
    FAT_BASE_BLOCK = BASE_PARTITION_BLOCK_SIZE + n_reserved_sectors;
    DATA_BASE_BLOCK = FAT_BASE_BLOCK + n_fat_tables * sectors_per_fat;

    uart_puts("FAT32 :\n");
    uart_puts("\tBytes per logical sector:");
    uart_puts_i(bytes_per_logical_sector);
    uart_puts("\n\tSectors per cluster:");
    uart_puts_i(sectors_per_cluster);
    uart_puts("\n\tNumber of FAT tables:");
    uart_puts_i(n_fat_tables);
    uart_puts("\n\tRoot cluster number:");
    uart_puts_i(root_start_cluster);
    uart_puts("\n\tSectors per FAT table:");
    uart_puts_i(sectors_per_fat);
    uart_puts("\n\tReserved sectors number:");
    uart_puts_i(n_reserved_sectors);
    uart_puts("\n==============================\n");

    sd_init_fs(mount->root);

    return 0;
}

int fat32_lookup(struct vnode * dir_node, struct vnode ** target, const char * component_name)
{
    struct vnode * fat_node = dir_node;

    while(dir_node->internel != NULL && ((struct fat32_internel *)(fat_node->internel))->next_sibling != NULL)
    {
        if (!strcmp(component_name, ((struct fat32_internel *)(fat_node->internel))->name))
        {
            break;
        }
        fat_node = ((struct fat32_internel *)(fat_node->internel))->next_sibling;
    }

    if (fat_node->internel == NULL || strcmp(component_name, ((struct fat32_internel *)(fat_node->internel))->name))
    {
        return -1;
    }
    else
    {
        *target = fat_node;
        return 0;
    }
}

int fat32_create(struct vnode * dir_node, struct vnode ** target, const char * component_name)
{
    return -1;
}

int fat32_read(struct file * file, void * buf, size_t len)
{
    struct fat32_internel * internel = file->vnode->internel;
    int start_cluster = internel->start_cluster;

    int target_block = DATA_BASE_BLOCK + start_cluster + (file->f_pos / FAT_BLOCK_SIZE);
    int offset = file->f_pos % FAT_BLOCK_SIZE;

    char * buf_addr = (char *)buf;
    char buffer[FAT_BLOCK_SIZE];
    readblock(target_block, buffer);

    for(int i = 0; i < len; ++i)
    {
        buf_addr[i] = buffer[offset + i];
    }

    file->f_pos += len;

    return len;
}

int fat32_write(struct file * file, const void * buf, size_t len)
{
    struct fat32_internel * internel = file->vnode->internel;
    int start_cluster = internel->start_cluster;

    int target_block = DATA_BASE_BLOCK + start_cluster + (file->f_pos / FAT_BLOCK_SIZE);
    int offset = file->f_pos % FAT_BLOCK_SIZE;

    char * buf_addr = (char *)buf;
    char buffer[FAT_BLOCK_SIZE];
    readblock(target_block, buffer);

    for (int i = 0; i < len; ++i)
    {
        buffer[offset + i] = buf_addr[i];
    }
    writeblock(target_block, buffer);

    file->f_pos += len;

    if (file->f_pos > internel->size)
    {
        internel->size = file->f_pos;
        fat32_update_filesize(file->vnode, internel->size);
    }

    return len;
}

struct vnode * fat32_create_vnode(struct vnode * src, const char * name, int size, int start_cluster, int entry_index)
{
    struct vnode * node = (struct vnode *)buddy_alloc(sizeof(struct vnode));
    
    node->mount = src->mount;
    node->v_ops = src->v_ops;
    node->f_ops = src->f_ops;
    node->internel = (void *)fat32_create_internel(name, size, start_cluster, entry_index);

    return node;
}

struct fat32_internel * fat32_create_internel(const char * name, int size, int start_cluster, int entry_index)
{
    struct fat32_internel * internel = (struct fat32_internel *)buddy_alloc((sizeof(struct fat32_internel)));

    strcpy(name, internel->name);
    internel->size = size;
    internel->start_cluster = start_cluster;
    internel->entry_index = entry_index;
    internel->next_sibling = NULL;

    return internel;
}

void fat32_append_child(struct vnode * parent, struct vnode * child)
{
    struct fat32_internel * internel = (struct fat32_internel *)(parent->internel);

    if (internel == NULL)
    {
        *parent = *child;
    }
    else
    {
        while(internel->next_sibling != NULL)
        {
            if (internel->next_sibling->internel != NULL)
            {
                internel = (struct fat32_internel *)(internel->next_sibling->internel);
            }
            else
            {
                break;
            }
        }
        internel->next_sibling = child;
    }
}

void sd_init_fs(struct vnode * root)
{
    char buffer[FAT_BLOCK_SIZE];

    readblock(DATA_BASE_BLOCK, buffer);

    int offset = 0, entry_index = 0;
    while (buffer[offset] != 0 && buffer[offset] != 0xE5)
    {
        // filename
        int i;
        char filename[13] = { 0 };
        // name
        for (i = 0; i < 8; ++i)
        {
            filename[i] = buffer[offset + i];
            if (filename[i] == ' ') break;
        }
        filename[i] = '.';
        // ext
        for (int j = 0; j < 3; ++i, ++j)
        {
            filename[i] = buffer[offset + 8 + j];
            if (filename[i] == ' ') break;
        }
        filename[i] = '\0';

        int filesize = *((int *)&buffer[offset + 0x1C]);

        int cluster_index;
        cluster_index = *((short *)&buffer[offset + 0x14]);
        cluster_index = (cluster_index << 16) + *((short *)&buffer[offset + 0x1A]);

        struct vnode * node = fat32_create_vnode(root, filename, filesize, cluster_index, entry_index);

        fat32_append_child(root, node);

        offset += 32;
        entry_index++;
    }
}

void fat32_update_filesize(struct vnode * node, int size)
{
    struct fat32_internel * internel = node->internel;
    char buffer[512];
    readblock(DATA_BASE_BLOCK, buffer);

    int * size_addr = (int *)&(buffer[32 * internel->entry_index + 0x1C]);
    *size_addr = size;

    writeblock(DATA_BASE_BLOCK, buffer);
}