#include "include/vfs.h"
#include "include/fat32.h"
#include "include/sdhost.h"
#include "include/cutils.h"
#include "include/mini_uart.h"



struct fat32_meta meta;
char FAT[512];
struct fat32_ent fat32_root;
struct fat32_ent fat32ent_pool[MAX_FAT32ENT_NR];


void init_fat32ent_pool()
{
    for (int i = 0; i < MAX_FAT32ENT_NR; ++i)
        fat32ent_pool[i].flag = 3;
}

struct fat32_ent *new_fat32ent()
{
    struct fat32_ent *fat32ent;
    for (int i = 0; i < MAX_FAT32ENT_NR; ++i) {
        fat32ent = &fat32ent_pool[i];
        if (fat32ent->flag & ENT_FREE) {
            fat32ent->flag &= ~ENT_FREE;
            fat32ent->rsib = 0;
            fat32ent->parent = 0;
            fat32ent->child = 0;
            fat32ent->first_cluster = 0;
            return fat32ent;
        }
    }
    return 0; // NULL
}

unsigned long cluster_to_lba(unsigned long cluster)
{
    return meta.cluster_begin_lba + (cluster - 2) * meta.sectors_per_cluster;
}



int fat_get_num(char *start, int len)
{
    int res, tmp;

    res = 0;
    for (int i = 0; i < len; ++i) {
        tmp = (int)start[i] << (8 * i);
        res += tmp;
    }
    return res;
}

int fat32_insert_dirent(struct fat32_ent *dir, struct fat32_ent *child)
{
    struct fat32_ent *walk;

    child->parent = dir;
    if ((walk = dir->child)) {
        while (walk) {
            if (!(walk->rsib)) {
                walk->rsib = child;
                return 0;
            }
            walk = walk->rsib;
        }
    } else { // child is the first child of dir.
        dir->child = child;
    }

    return 0;
}

int fat32_get_next_cluster(int clstr)
{
    int fat_lba;
    char *fat_ent;

    fat_lba = meta.fat_begin_lba + (clstr >> 7);
    readblock(fat_lba, FAT);
    fat_ent = FAT + (clstr & 0x7F) * 4;

    return fat_get_num(fat_ent, 4);
}


int fat32_read_pos_clstr_to_buf(char *buf, int f_pos, struct fat32_ent *ent)
{
    int clstr_num, clstr_tmp;

    clstr_num = f_pos / 512;
    clstr_tmp = ent->first_cluster;
    while (clstr_num) {
        clstr_tmp = fat32_get_next_cluster(clstr_tmp);
        clstr_num--;
    }
    readblock(cluster_to_lba(clstr_tmp), buf);
    
    return clstr_tmp;
}


int fat32_parse_name(char *buf, char *fat32_dirent)
{
    char *ch, *bp;
    int cnt;

    bp = buf;
    /* Name */
    cnt = 0;
    ch = fat32_dirent;
    while (*ch != ' ' && cnt < 8) {
        *bp++ = *ch++;
        cnt++;
    }
    *bp++ = '.';

    /* Extension */
    cnt = 0;
    ch = fat32_dirent + 8;
    while (*ch != ' ' && cnt < 3) {
        *bp++ = *ch++;
        cnt++;
    }
    *bp = '\0';

    return 0;
}

int fat32_write(struct file* file, const void* buf, int len)
{ // Assume file size wouldn't grow over 4096 bytes.
    struct fat32_ent *tar;
    int inc, clstr;
    char sector[512], buf_name[13];    
    char *fat32_dirent;

    tar = (struct fat32_ent*)(file->vnode->internal);
    // TODO: from pos to pos + len could cross sector
    // so now we assume user won't do that.
    // if (tar->type == DIRENT)
    //     return 0;
    inc = file->f_pos + len - tar->size;
    if (inc) {
        tar->size += inc;
        // Update dir's meta of this file.
        readblock(cluster_to_lba(tar->parent->first_cluster), sector);
        fat32_dirent = (char*)sector;
        while (*fat32_dirent) { // while not end of dir
            // TODO: For simplicity, we assume all 
            // dirent can be found in first cluster.
            if (*fat32_dirent == 0xE5) { // unused entry
                fat32_dirent += 32;
                continue;
            }
            fat32_parse_name(buf_name, fat32_dirent);
            if (!strcmp(buf_name, tar->name)) {
                *((int*)&fat32_dirent[28]) = tar->size;
                writeblock(cluster_to_lba(tar->parent->first_cluster), sector);
                break;
            }
            fat32_dirent += 32;
        }
    }

    clstr = fat32_read_pos_clstr_to_buf(sector, file->f_pos, tar);

    memcpy(sector + (file->f_pos % 512), (char*)buf, len);
    writeblock(cluster_to_lba(clstr), sector);
    file->f_pos += len;

    return len;
}

int fat32_read(struct file* file, void* buf, int len)
{
    struct fat32_ent *tar;
    int remain, len_r;
    char sector[512];

    tar = (struct fat32_ent*)(file->vnode->internal);
    
    // TODO: from pos to pos + len could cross sector
    // so now we assume user won't do that.
    remain = tar->size - file->f_pos;
    len_r = remain >= len ? len : remain;
    fat32_read_pos_clstr_to_buf(sector, file->f_pos, tar);

    memcpy((char*)buf, sector + (file->f_pos % 512), len_r);
    file->f_pos += len_r;
    
    return len_r;
}

int fat32_component_to_sfn(char *buf, char *component_name)
{
    char *ch, *np;

    for (int i = 0; i < 11; ++i) // Padding all byte in 8.3 with space.
        buf[i] = ' ';
    // TODO: For simplicity, we assume new created file's length
    // won't exceed 8.3
    ch = component_name;
    np = buf;
    while (*ch != '\0' && *ch != '.')
        *np++ = *ch++;
    if (*ch == '.') {
        np = &buf[8];
        ch++;
        while (*ch != '\0')
            *np++ = *ch++;
    }
    return 0;
}

int fat32_create(struct vnode* dir_node, struct vnode** target,
    const char* component_name, int type)
{
    struct fat32_ent *dir, *new;
    char *fat32_dirent;
    char sector[512];
    int first_cluster;

    dir = (struct fat32_ent*)(dir_node->internal);

    /* Find FAT's empty entry */
        // TODO: We assume empty entry can be found in
        // first cluster of FAT.
    int *fat_ent;
    readblock(meta.fat_begin_lba, FAT);
    fat_ent = (int*)FAT;
    while (*fat_ent)
        fat_ent++;
    *fat_ent = 0xFFFFFFFF;
    writeblock(meta.fat_begin_lba, FAT);
    first_cluster = ((unsigned long)fat_ent - (unsigned long)FAT) >> 2;

    /* Find dir's unused direntry */
        // TODO: For simplicity, we assume all 
        // dirent can be found in first cluster.
    readblock(cluster_to_lba(dir->first_cluster), sector);
    fat32_dirent = (char*)sector;
    while (1) {
        if (fat32_dirent[0] == 0xE5) { // unused entry
            break;
        }
        if (!fat32_dirent[0]) { // EOD (end of directory)
            (fat32_dirent + 32)[0] = 0; // Make next entry be EOD
            break;
        }
        fat32_dirent += 32;
    }
        // Setting the unused direntry
    fat32_component_to_sfn(fat32_dirent, component_name);
    fat32_dirent[0x0B] = type == REG_DIR ? 0x10 : 0x0; // AttribByte
    *((int*)&fat32_dirent[0x1C]) = 0; // Size
    fat32_dirent[20] = (char)(first_cluster >> (8 * 2)); // ClusterHigh
    fat32_dirent[21] = (char)(first_cluster >> (8 * 3));
    fat32_dirent[26] = (char)(first_cluster >> (8 * 0)); // ClusterLow
    fat32_dirent[27] = (char)(first_cluster >> (8 * 1));
    writeblock(cluster_to_lba(dir->first_cluster), sector);


    new = new_fat32ent();
    // Setting name
    strcpy(new->name, component_name); // TODO: name format
    // Add into directory
    fat32_insert_dirent(dir, new);
    // Setting vnode
    new->vnode = new_vnode();
    *target = new->vnode;
    (*target)->mount = dir_node->mount;
    (*target)->v_ops = dir_node->v_ops;
    (*target)->f_ops = dir_node->f_ops;
    (*target)->internal = new;
    // Setting name, type, size, first_cluster
    strcpy(new->name, component_name);
    new->type = type;
    new->size = 0;
    new->first_cluster = first_cluster;

    return 0;
}


int fat32_lookup(struct vnode* dir_node, struct vnode** target,
    const char* component_name)
{
    struct fat32_ent *ent, *walk, *new;
    char *fat32_dirent;
    char sector[512];

    if (dir_node->by) // dir_node is mounted by someone
        return dir_node->by->v_ops->lookup(dir_node->by, target, component_name);

    ent = (struct fat32_ent*)(dir_node->internal);
    if (ent->type != DIRENT)
        uart_send_string("Error: dir_node is not a directory\r\n");

    if (ent->flag & ENT_FIRST_VISIT) { // direntry hasn't been created.
        ent->flag &= ~ENT_FIRST_VISIT;
        readblock(cluster_to_lba(ent->first_cluster), sector);
        fat32_dirent = (char*)sector;
        while (fat32_dirent[0]) { // while not end of dir
        // TODO: For simplicity, we assume all 
        // dirent can be found in first cluster.
            if (fat32_dirent[0] == 0xE5) { // unused entry
                fat32_dirent += 32;
                continue;
            }
            /* Setting up new fat32_ent */
            new = new_fat32ent();
            /* Setting vnode */
            new->vnode = new_vnode();
            new->vnode->mount = dir_node->mount;
            new->vnode->v_ops = dir_node->v_ops;
            new->vnode->f_ops = dir_node->f_ops;
            new->vnode->internal = new;
            /* Setting name, type, size, first_cluster */
            fat32_parse_name(new->name, fat32_dirent);
            new->type = fat32_dirent[0x0B] & 0x10 ? DIRENT : FILEENT; // Attri byte & dir_flag
            // new->size = fat_get_num(&fat32_dirent[28], 4);
            new->size = *((int*)&fat32_dirent[0x1C]);
            new->first_cluster = ((int)fat32_dirent[20] << (8 * 2)) +
                                ((int)fat32_dirent[21] << (8 * 3)) +
                                ((int)fat32_dirent[26] << (8 * 0)) +
                                ((int)fat32_dirent[27] << (8 * 1));
            /* Handle relation between family */
            fat32_insert_dirent(ent, new);

            fat32_dirent += 32;
        }
    }
    // Every node here has it's direntry created already.
    walk = ent->child;
    while (walk) {
        if (!strcmp(walk->name, component_name)) {
            *target = walk->vnode;
            return 0;
        }
        walk = walk->rsib;
    }
    return 1;
}

int fat32_get_parent(struct vnode* dir_node, struct vnode** target)
{
    struct fat32_ent *tmpdir;

    tmpdir = (struct fat32_ent*)(dir_node->internal);
    if (tmpdir == tmpdir->parent) { // means tmpdir is a root
        if (!(dir_node->on)) {
            *target = dir_node;
            return 0;
        }
        // dir_node is on someone.
        return dir_node->on->v_ops->get_parent(dir_node->on, target);
    }
    *target = tmpdir->parent->vnode;
    return 0;
}

int fat32_stat(struct vnode *vnode, struct dentry *dent)
{
    struct fat32_ent *self;

    self = (struct fat32_ent*)(vnode->internal);
    strcpy(dent->name, self->name);
    dent->size = self->size;
    dent->type = self->type;
    return 0;
}

int fat32_get_rsib(struct vnode *vnode, struct vnode **target)
{
    struct fat32_ent *self, *rsib;

    self = (struct fat32_ent*)(vnode->internal);
    if (!(rsib = self->rsib))
        *target = 0;
    else
        *target = rsib->vnode; // may be NULL
    return 0;
}

int fat32_get_child(struct vnode *vnode, struct vnode **target)
{
    struct fat32_ent *self;

    if (vnode->by)
        return vnode->by->v_ops->get_child(vnode->by, target);
    self = (struct fat32_ent*)(vnode->internal);
    if (!(self->child))
        *target = 0;
    else
        *target = self->child->vnode; // may be NULL
    return 0;
}

int fat32_setup_mount(struct filesystem* fs,
    struct mount* mount, struct vnode *root)
{
    mount->root = root;
    mount->fs = fs;
    mount->root->mount = mount;
    mount->root->type = REG_DIR;
    mount->root->internal = (void*)&fat32_root;
    if (!(mount->fs->cnt++)) // only firt time setup will set root.vnode
        fat32_root.vnode = mount->root;
    mount->root->f_ops->read = fat32_read;
    mount->root->f_ops->write = fat32_write;
    mount->root->v_ops->create = fat32_create;
    mount->root->v_ops->lookup = fat32_lookup;
    mount->root->v_ops->get_parent = fat32_get_parent;
    mount->root->v_ops->stat = fat32_stat;
    mount->root->v_ops->get_rsib = fat32_get_rsib;
    mount->root->v_ops->get_child = fat32_get_child;

    // TODO: This is just a temporary solution
    // to populate root. By using fat32_lookup,
    // we can populate root cause it is a 
    // first-visit-vnode.
    struct vnode useless;
    struct vnode *useless_p = &useless;
    fat32_lookup(fat32_root.vnode, &useless_p, ".");
    return 0;
}

int init_fat32()
{
    char sector[512];
    char volume_id[512];
    int Signature, Partition_LBA_Begin;

    readblock(0, sector);
    if (!(sector[510] == 0x55) || !(sector[511] == 0xAA))
        uart_send_string("Error: MBR panic\r\n");
    char *partition1_base = &sector[446];
    if ((partition1_base[4] != 0x0b) && (partition1_base[4] != 0x0c)) // Type Code
        uart_send_string("Error: partition1 panic\r\n");

    Partition_LBA_Begin = fat_get_num(partition1_base+8, 4);
    readblock(Partition_LBA_Begin, volume_id);

    meta.Number_of_Reserved_Sectors = fat_get_num(volume_id + 0x0E, 2);
    meta.Number_of_FATs = fat_get_num(volume_id + 0x10, 1);
    meta.Sectors_Per_FAT = fat_get_num(volume_id + 0x24, 4);
    meta.fat_begin_lba = Partition_LBA_Begin + meta.Number_of_Reserved_Sectors;
    meta.cluster_begin_lba = Partition_LBA_Begin + meta.Number_of_Reserved_Sectors + (meta.Number_of_FATs * meta.Sectors_Per_FAT);
    meta.sectors_per_cluster = fat_get_num(volume_id + 0x0D, 1);
    meta.root_dir_first_cluster = fat_get_num(volume_id + 0x2C, 4);
    Signature = fat_get_num(volume_id + 0x1FE, 2);

    if ((Signature != 0xAA55) || (meta.Number_of_FATs != 2))
        uart_send_string("Error: Volume ID panic\r\n");

    // readblock(fat_begin_lba, FAT); // Cache first sector of FAT

    init_fat32ent_pool();

    fat32_root.parent = &fat32_root;
    fat32_root.rsib = &fat32_root;
    fat32_root.type = DIRENT;
    fat32_root.flag = 2;
    fat32_root.first_cluster = meta.root_dir_first_cluster;
    strcpy(fat32_root.name, "/");
    // readblock(cluster_to_lba(meta.root_dir_first_cluster), sector);

    // fat32_root.page_cache = kmalloc(0x1000);
    return 0;
}

