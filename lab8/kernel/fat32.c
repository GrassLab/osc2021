#include "fat32.h"

struct fat32_meta sd_p1;
struct filesystem fat32fs = {.name = "fat32fs", .setup_mount = setup_mount};
static struct file_operations fat32_f_ops = {.write = write, .read = read, .filesize = filesize, .file_name = node_name, .content = NULL};
static struct vnode_operations fat32_v_ops = {.lookup = lookup, .create_file = NULL, .create_dir = NULL, .list = list, .dir_name = node_name, .content = NULL, .set_parent = NULL};

static int setup_mount(struct filesystem *fs, struct mount *mount){
    mount->root = malloc(sizeof(struct vnode));
    if(!mount->root){
        return -1;
    }
    mount->root->mount = mount;
    mount->root->v_ops = &fat32_v_ops;
    mount->root->f_ops = NULL;
    mount->root->internal = malloc(sizeof(struct fat32_dir));
    if(!mount->root->internal){
        free(mount->root);
        return -1;
    }

    struct fat32_dir *root_dir = mount->root->internal;
    strncpy(root_dir->name, "/       ", 8);
    strncpy(root_dir->ext, "   ", 3);
    root_dir->attr = FAT32_DIR;
    root_dir->cluster_high = sd_p1.root_dir_first_cluster >> 16;
    root_dir->cluster_low = sd_p1.root_dir_first_cluster & 0xFF;
    root_dir->size = 0;

    mount->fs = fs;
    return 0;
}

static int lookup(struct vnode *dir_node, struct vnode **target, const char *component_name){
    struct fat32_dir *dir = (struct fat32_dir *)dir_node->internal;
    struct fat32_dir dir_list[SECTOR_SIZE / sizeof(struct fat32_dir)];
    char filename[16];
    struct vnode temp;
    readblock(cluster_to_lba(&sd_p1, JOIN_16(dir->cluster_high, dir->cluster_low)), dir_list);
    for(size_t i=0; i<SECTOR_SIZE / sizeof(struct fat32_dir); i++){
        if((dir_list[i].name[0] == 0) || (dir_list[i].name[0] == 0xE5) || ((dir_list[i].attr & 0x0F) == 0x0F)){
            continue;
        }
        temp.internal = &dir_list[i];
        node_name(&temp, filename);
        if(!strcmp(filename, component_name)){
            (*target) = malloc(sizeof(struct vnode));
            if(!(*target)){
                return -1;
            }
            (*target)->internal = malloc(sizeof(struct fat32_dir));
            if(!(*target)->internal){
                free(*target);
                return -1;
            }

            (*target)->mount = NULL;
            strncpy((*target)->internal, (char *)&dir_list[i], sizeof(struct fat32_dir));
            if(dir_list[i].attr & FAT32_DIR){
                (*target)->v_ops = &fat32_v_ops;
                (*target)->f_ops = NULL;
            }else{
                (*target)->v_ops = NULL;
                (*target)->f_ops = &fat32_f_ops;
            }
            return 0;
        }
    }
    return -1;
}

//static int create(struct vnode* dir_node, struct vnode **target, const char *component_name, uint64_t mode);
//static int create_file(struct vnode* dir_node, struct vnode **target, const char *component_name);
//static int create_dir(struct vnode* dir_node, const char *component_name);

static int write(struct file *file, const void *buf, size_t len){
    struct fat32_dir *node = file->vnode->internal;

    //TODO
    if(node->size + len > SECTOR_SIZE){
        return -1;
    }
    //TODO allocate new sector
    if(file->f_pos + len > ROUNDUP_MUL512(node->size)){
        return -1;
    }

    uint32_t nsector = file->f_pos / SECTOR_SIZE;
    uint32_t secter_offset = file->f_pos % SECTOR_SIZE;

    uint8_t sector[SECTOR_SIZE];
    readblock(cluster_to_lba(&sd_p1, JOIN_16(node->cluster_high, node->cluster_low) + nsector), sector);
    strncpy(sector + secter_offset, buf, len);

    //all file on root
    struct fat32_dir dir_list[SECTOR_SIZE / sizeof(struct fat32_dir)];
    readblock(sd_p1.cluster_begin_lba, dir_list);
    for(size_t i=0; i<SECTOR_SIZE / sizeof(struct fat32_dir); i++){
        if((dir_list[i].name[0] == 0) || (dir_list[i].name[0] == 0xE5) || ((dir_list[i].attr & 0x0F) == 0x0F)){
            continue;
        }
        if(!strncmp(dir_list[i].name, ((struct fat32_dir *)file->vnode->internal)->name, 8) &&
            !strncmp(dir_list[i].ext, ((struct fat32_dir *)file->vnode->internal)->ext, 3)
        ){
            writeblock(cluster_to_lba(&sd_p1, JOIN_16(node->cluster_high, node->cluster_low) + nsector), sector);
            file->f_pos += len;
            node->size = file->f_pos;

            dir_list[i].size = file->f_pos;
            writeblock(sd_p1.cluster_begin_lba, dir_list);
            return len;
        }
    }

    return -1;
}

static int read(struct file *file, void *buf, size_t len){
    struct fat32_dir *node = file->vnode->internal;

    //TODO
    if(node->size > SECTOR_SIZE){
        return -1;
    }
    if(file->f_pos == node->size){
        return -22;
    }
    if(file->f_pos + len > node->size){
        len = node->size - file->f_pos;
    }

    uint32_t nsector = file->f_pos / SECTOR_SIZE;
    uint32_t secter_offset = file->f_pos % SECTOR_SIZE;

    uint8_t sector[SECTOR_SIZE];
    readblock(cluster_to_lba(&sd_p1, JOIN_16(node->cluster_high, node->cluster_low) + nsector), &sector);
    strncpy(buf, sector + secter_offset, len);
    file->f_pos += len;
    return len;
}

static size_t filesize(struct file *file){
    return ((struct fat32_dir *)file->vnode->internal)->size;
}

static void list(struct vnode *dir_node){
    struct fat32_dir *dir = (struct fat32_dir *)dir_node->internal;
    struct fat32_dir dir_list[SECTOR_SIZE / sizeof(struct fat32_dir)];
    readblock(cluster_to_lba(&sd_p1, JOIN_16(dir->cluster_high, dir->cluster_low)), dir_list);
    char filename[16];
    struct vnode temp;
    for(size_t i=0; i<SECTOR_SIZE / sizeof(struct fat32_dir); i++){
        if((dir_list[i].name[0] == 0) || (dir_list[i].name[0] == 0xE5) || ((dir_list[i].attr & 0x0F) == 0x0F)){
            continue;
        }
        temp.internal = &dir_list[i];
        node_name(&temp, filename);
        if(dir_list[i].attr & FAT32_DIR){
            printf("  d  %lu  %s" NEW_LINE, 0, filename);
        }else{
            printf("  f  %lu  %s" NEW_LINE, dir_list[i].size, filename);
        }
    }
}

static void node_name(struct vnode *vnode, char *buf){
    struct fat32_dir *node = (struct fat32_dir *)vnode->internal;

    char name[9];
    name[8] = '\0';
    for(size_t i=0; i<8; i++){
        if(node->name[i] == ' '){
            name[i] = '\0';
            break;
        }
        name[i] = node->name[i];
    }

    char ext[4];
    ext[3] = '\0';
    for(size_t i=0; i<3; i++){
        if(node->ext[i] == ' '){
            ext[i] = '\0';
            break;
        }
        ext[i] = node->ext[i];
    }

    strncpy(buf, name, -1);
    if(ext[0] != '\0'){
        strcat(buf, ".");
        strcat(buf, ext);
    }
}

//static void *content(struct vnode *vnode);
//static void set_parent(struct vnode *mount, struct vnode *mountpoint);

static void dump_hex(void *buf, size_t size){
    for(size_t i=0; i<size; i++){
        uint8_t num = ((uint8_t *)buf)[i];
        uint8_t rem = num % 16;
        num /= 16;
        miniuart_send_C(((num > 9)? (num - 10) + 'a' : num + '0'));
        miniuart_send_C(((rem > 9)? (rem - 10) + 'a' : rem + '0'));
        if(i % 16 == 15){
            miniuart_send_S(NEW_LINE);
        }else if(i % 2 == 1){
            miniuart_send_C(' ');
        }
    }
    if(size % 16 != 0){
        miniuart_send_S(NEW_LINE);
    }
    miniuart_send_S(NEW_LINE);
}

static uint32_t get_fat_value(struct fat32_meta *meta, uint32_t index){
    uint32_t fat_page = index / 128;
    index %= 128;
    uint32_t fat[SECTOR_SIZE / 4];
    readblock(meta->fat_begin_lba + fat_page, &fat);
    return fat[index] & 0x0FFFFFFF;
}


static uint32_t lba_to_cluster(struct fat32_meta *meta, size_t lba){
    return ((lba - meta->cluster_begin_lba) / meta->sectors_per_cluster) + 2;
}

static uint32_t cluster_to_lba(struct fat32_meta *meta, size_t cluster){
    return meta->cluster_begin_lba + (cluster - 2) * meta->sectors_per_cluster;
}

int32_t fat32_init(){
    struct mbr mbr;
    readblock(0, &mbr);

    struct partition_entry *p1 = &mbr.p1;
    if(p1->type != 0x0B && p1->type != 0x0C){
        printf("Error: SD card partition one is not FAT32!!!" NEW_LINE);
        return -1;
    }

    uint32_t lba_first;
    strncpy((char *)&lba_first, (char *)&p1->lba_first, 4);

    struct boot_sector bs;
    readblock(lba_first, &bs);

    struct bios_parameter_blk *bpb = &bs.bpb;
    sd_p1.fat_begin_lba = lba_first + bpb->num_reserved_logical_sectors;
    sd_p1.cluster_begin_lba = sd_p1.fat_begin_lba + (bpb->num_fat * bpb->logical_sectors_per_fat);
    sd_p1.sectors_per_cluster = bpb->logical_sectors_per_cluster;
    sd_p1.root_dir_first_cluster = bpb->cluster_num_of_root;

    return 0;
}
