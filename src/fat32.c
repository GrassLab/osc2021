#include "fat32.h"
#include "slab.h"
#include "vfs.h"
#include "printf.h"

fat32_partition_t *sd_root_partition;
fat32_boot_sector_t *sd_root_bst;
extern struct mount *rootfs_mount;
#define CONCATE_8( a, b ) ( ( a ) | ( b << 8 ) )

static int strncmp(char *str1, char *str2, int num) {
    for(int i = 0 ; i < num ; i++) {
        char a = str1[i];
        char b = str2[i];
        a = a == 32 ? 0 : a;
        b = b == 32 ? 0 : b;
        if (a == 0 && b == 0) break;
        a = str1[i] >= 'A' && str1[i] <= 'Z' ? str1[i] : str1[i] - 'a' + 'A';
        b = str2[i] >= 'A' && str2[i] <= 'Z' ? str2[i] : str2[i] - 'a' + 'A';
        if(a != b) {
            return 0;
        }
    }
    return 1;
}

static void insert_vnode(struct directory *dir, struct vnode *in_vnode, char *name) {
    struct direntry *create_entry = (struct direntry *)obj_allocate(sizeof(struct direntry));
    create_entry->name = (char *)obj_allocate(sizeof(char) * 10);
    memcpy(name, create_entry->name, 8);
    fat32_node_t * node = (fat32_node_t *)(in_vnode->internal);
    create_entry->entry = in_vnode;
    if(dir->head == 0) {
        dir->head = create_entry;
        dir->head->next = 0;
        dir->tail = dir->head;
    }
    else {
        dir->tail->next = create_entry;
        dir->tail = dir->tail->next;
        dir->tail->next = 0;
    }
}

int fat32_write(struct file* file, const void* buf, int len) {
    if(file == 0) {
        return 0;
    }
    int f_pos = file->f_pos;
    fat32_dir_t *parent_dir;
    fat32_node_t *file_node;
    fat32_node_t *dir_node;
    struct direntry *iter_entry;
    char dir_entrty[512];
    char write_sector_buf[512];
    int i = 0;
    file_node = (fat32_node_t *)(file->vnode->internal);
    for(int i = 0 ; i < len ; i++) {
        write_sector_buf[i] = *(char *)(((char *)buf)+i);
    }
    writeblock ((file_node->cluster - 2 ) * sd_root_bst->logic_sector_per_cluster + sd_root_partition->starting_sector + sd_root_partition->root_sector_abs, write_sector_buf);
    // udpate size in directory entry
    dir_node = (fat32_node_t*)(((struct directory *)rootfs_mount->root->internal)->internal);
    readblock((dir_node->cluster - 2) * sd_root_bst->logic_sector_per_cluster + sd_root_partition->starting_sector + sd_root_partition->root_sector_abs, dir_entrty);
    parent_dir = (fat32_dir_t *) dir_entrty;
    iter_entry = ((struct directory *)rootfs_mount->root->internal)->head;
    while(iter_entry != 0) {
        if (strncmp(parent_dir[i].name, file_node->name, 8)) {
            printf("Change the file: %s\r\n", parent_dir[i].name);
            printf("Change the file size: %d to %d bytes.\r\n", parent_dir[i].size, len);
            parent_dir[i].size = len;
            break;
        }
        iter_entry = iter_entry->next;
        i += 1;
    }
    file_node->size = len;
    writeblock(( dir_node->cluster - 2 ) * sd_root_bst->logic_sector_per_cluster + sd_root_partition->starting_sector + sd_root_partition->root_sector_abs, dir_entrty);
    printf( "Write to the file: %s,size: %d bytes.\r\n", file_node->name, len);
    return len;
}

int fat32_read(struct file* file, void* buf, int len) {
    if(file == 0) {
        return 0;
    }
    int f_pos = file->f_pos;
    fat32_node_t * node;
    char read_sector_buf[512];
    node = (fat32_node_t *)(file->vnode->internal);
    readblock ( ( node->cluster - 2 ) * sd_root_bst->logic_sector_per_cluster + sd_root_partition->starting_sector + sd_root_partition->root_sector_abs, read_sector_buf );
    if (len > (node->size - file->f_pos)) {
        len = node->size - file->f_pos;
    }
    memcpy(read_sector_buf + file->f_pos, buf, len);
    ((char *)buf)[len] = '\0';
    ( file->f_pos ) += len;
    printf( "Read from file: %s,read_size: %d bytes.\r\n", node->name, len );
    return len;
}

int fat32_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name) { 
    struct directory *read_dir  = (struct directory *)dir_node->internal;
    struct direntry *iter_entry = read_dir->head;
    if(iter_entry == 0) {
        return 0;
    }
    while(iter_entry != 0) {
        if((iter_entry->entry->type == REG_FILE) && strncmp(((struct direntry*)iter_entry)->name, component_name, 8)) {
            *target = iter_entry->entry;
            return 1;
        }
        iter_entry = iter_entry->next;
    }
    return 0;
}

void fat32_setup_vnode(struct vnode *new_vnode, struct mount *mount) {
    new_vnode->v_ops = (struct vnode_operations *)obj_allocate(sizeof(struct vnode_operations));
    new_vnode->f_ops = (struct file_operations *)obj_allocate(sizeof(struct file_operations));
    new_vnode->mount = mount;
    new_vnode->v_ops->lookup = fat32_lookup;
    new_vnode->v_ops->create = 0;
    new_vnode->f_ops->write  = fat32_write;
    new_vnode->f_ops->read   = fat32_read;
}

int fat32_setup_mount(struct filesystem* fs, struct mount* mount) {
    char *sector;
    fat32_dir_t * dir;
    fat32_node_t * node;
    mount->root                = (struct vnode *)obj_allocate(sizeof(struct vnode));
    mount->fs                  = fs;
    mount->root->mount         = mount;
    mount->root->type          = REG_DIR;
    mount->root->internal      = (struct directory *)obj_allocate(sizeof(struct directory));
    mount->root->f_ops         = (struct file_operations *)obj_allocate(sizeof(struct file_operations));
    mount->root->f_ops->read   = fat32_read;
    mount->root->f_ops->write  = fat32_write;
    mount->root->v_ops         = (struct vnode_operations *)obj_allocate(sizeof(struct vnode_operations));
    mount->root->v_ops->create = 0;
    mount->root->v_ops->lookup = fat32_lookup;
    ((struct directory *)mount->root->internal)->internal = (fat32_node_t*)obj_allocate(sizeof(fat32_node_t));
    ((fat32_node_t*)((struct directory *)mount->root->internal)->internal)->name[0] =  '/';
    ((fat32_node_t*)((struct directory *)mount->root->internal)->internal)->name[1] =  '\0';
    ((fat32_node_t*)((struct directory *)mount->root->internal)->internal)->cluster =  2;
    // appned child
    sector = (char *)obj_allocate(sizeof(char) * 512);
    readblock( sd_root_partition->root_sector_abs + sd_root_partition->starting_sector, sector);
    dir    = (fat32_dir_t *) sector;
    printf("SD Mounted\r\n");
    printf("File under root directory.\r\n");
    for(int i = 0; dir[i].name[0] != '\0'; i++ ) {
        struct vnode *new_vnode = (struct vnode *)obj_allocate(sizeof(struct vnode)); 
        fat32_setup_vnode(new_vnode, mount);
        node = (fat32_node_t *)obj_allocate(sizeof(fat32_node_t));
        node->cluster = (((unsigned int)( dir[i].cluster_high ))<<16)|(dir[i].cluster_low);
        node->size    = dir[i].size;
        memcpy(dir[i].name, node->name, 8);
        node->name[8] = '\0';
        for (int i = 0 ; i < 3 ; i++) {
            node->ext[i] = dir[i].ext;
        }
        node->ext[3]  = '\0';
        insert_vnode((struct directory *)mount->root->internal, new_vnode, node->name);
        //set vnode type
        new_vnode->type  = dir[i].attr[0] & 16 ? REG_DIR : REG_FILE;
        new_vnode->internal = (void *)node;
        printf("%s\r\n", dir[i].name);
    }
    //append child
    return 1;
}

int fat_getpartition(void *buff) {
    sd_root_partition = (fat32_partition_t *)obj_allocate(sizeof(fat32_partition_t));
    unsigned char *mbr = (unsigned char *)buff;
    readblock(0, (void*)buff);
    if(mbr[510]!=0x55 || mbr[511]!=0xAA) {
        printf("ERROR: Bad magic in MBR\n");
        return 0;
    }
    printf("MBR disk status: %x\r\n", mbr[0x1be]);
    printf("Partition type: 0x%x\r\n", mbr[0x1be + 4]);
    printf("FAT partition starts at: %x\r\n", *(unsigned int *)&mbr[0x1be + 8]);
    for ( int i = 0; i < 16; i++ )
        ( (char *) sd_root_partition )[i] = mbr[446 + i];
    return 1;
}

void fat_listdirectory(void *buff) {
    readblock (sd_root_partition->starting_sector, buff);
    sd_root_bst = (fat32_boot_sector_t *) buff;
    char * sector = (char *)buff;
    sd_root_bst->bytes_per_logic_sector = CONCATE_8 ( sector[11], sector[12] );
    sd_root_bst->n_root_dir_entries     = CONCATE_8 ( sector[17], sector[18] );
    sd_root_partition->root_sector_abs  = ( sd_root_bst->n_sector_per_fat_32 * sd_root_bst->n_file_alloc_tabs ) + sd_root_bst->n_reserved_sectors;
    fat32_dir_t *dir   = (fat32_dir_t *) buff;
    readblock( sd_root_partition->root_sector_abs + sd_root_partition->starting_sector, buff );
    for(int i = 0 ; dir[i].name[0] != '\0' ; i++) {
        dir[i].name[8] = '\0';
        printf("%s\r\n", dir[i].name);
    }
}

void memory_dump(void *ptr) {
    unsigned long a,b,d;
    unsigned char c;
    for(a=(unsigned long)ptr;a<(unsigned long)ptr+512;a+=16) {
        printf("%05x",a-(unsigned long)ptr); printf(": ");
        for(b=a ; b<a+16 ; b+=2) {
            printf("%04x ", *(unsigned short *)b);
        }
        printf("\r\n");
    }
}

void setup_fat32(struct filesystem **fs, char *name) {
    char *sector = (char *)obj_allocate(sizeof(char) * 512);
    sd_root_partition = (fat32_partition_t *)obj_allocate(sizeof(fat32_partition_t));
    struct filesystem *fs_fat32 = (struct filesystem *)obj_allocate(sizeof(struct filesystem));

    readblock(0, sector);
    for(int i = 0; i < 16; i++)
        ((char *)sd_root_partition)[i] = sector[446 + i];
    readblock(sd_root_partition->starting_sector, sector);

    sd_root_bst = (fat32_boot_sector_t *)sector;
    sd_root_bst->bytes_per_logic_sector = CONCATE_8(sector[11], sector[12]);
    sd_root_bst->n_root_dir_entries     = CONCATE_8(sector[17], sector[18]);
    sd_root_partition->root_sector_abs = ( sd_root_bst->n_sector_per_fat_32 * sd_root_bst->n_file_alloc_tabs ) + sd_root_bst->n_reserved_sectors;
    fs_fat32->name = name;
    fs_fat32->setup_mount = fat32_setup_mount;
    *fs = fs_fat32;
}