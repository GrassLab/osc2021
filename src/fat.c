#include "fat.h"
#include "mm.h"
#include "utils.h"
#include "sdhost.h"

#define BLOCK_SIZE 512

typedef struct mbr_partition_entry {
    uint8_t   ignore[8];
    uint32_t  sector_addr;
    uint32_t  sector_num;
} mbr_pe_t;

typedef struct fat_boot_sector_part1 {
    uint16_t  bpb_byte_per_sec;
    uint8_t   bpb_sec_per_clu;
} fat_bs_p1_t;

typedef struct fat_boot_sector_part2 {
    uint16_t  bpb_rsvd_sec_cnt;
    uint8_t   bpb_num_fats;
} fat_bs_p2_t;

typedef struct fat32_boot_sector_part3 {
    uint32_t  bpb_tot_sec;
    uint32_t  bpb_fat_sz;
    uint16_t  bpb_ext_flags;
    uint16_t  bpb_fs_ver;
    uint32_t  bpb_root_clus;
    uint16_t  bpb_fs_info;
    uint16_t  bpb_bk_bootsec;
} fat32_bs_p3_t;

typedef struct {
    fat_bs_p1_t   p1;
    fat_bs_p2_t   p2;
    fat32_bs_p3_t p3;
} fat32_bs_t;

typedef struct fat32_dir_entry_part1 {
    uint8_t file_name[8];
    uint8_t extention[3];
    uint8_t attribute;
} fat32_de_p1_t;

typedef struct fat32_dir_entry_part2 {
    uint16_t h_clu_num;
    uint16_t time;
    uint16_t date;
    uint16_t l_clu_num;
    uint32_t size;
} fat32_de_p2_t;

typedef struct fat32_dir_entry {
    fat32_de_p1_t p1;
    fat32_de_p2_t p2;
} fat32_de_t;

typedef struct {
    uint32_t fat_sec;
    uint32_t head_sec;
    uint32_t dir_sec;
    uint16_t file_num;
    uint32_t clus_pos;
    uint32_t size;
    uint8_t  sec_per_clu;
    uint16_t byte_per_sec;
} fat32_file_t;

vnode_operations_t fat32_vop;
file_operations_t fat32_fop;

static inline uint32_t fat32_clus_to_sec(uint32_t head_sec, uint32_t clus_pos,
                                     uint8_t  sec_per_clu, uint8_t sec_count) {
    return head_sec + sec_per_clu * (clus_pos - 2) + sec_count;
}

static uint32_t fat32_get_next_clus(uint32_t current_clus, uint32_t fat_sec,
                                    uint16_t byte_per_sec) {
    char buf[BLOCK_SIZE];
    fat_sec += current_clus / (byte_per_sec / 4);
    current_clus %= (byte_per_sec / 4);
    readblock(fat_sec, buf);
    uint32_t *next_clus = &buf[4 * current_clus];
    if (*next_clus < 0xffffff8) {
        return *next_clus;
    } else {
        return 0;
    }
}

static void fat32_update_file_size(fat32_file_t *file) {
    char buf[BLOCK_SIZE];
    readblock(file->dir_sec, buf);
    uint32_t *size = &buf[file->file_num * 32 + 28];
    *size = file->size;
    writeblock(file->dir_sec, buf);
}

static void fat32_umount_operation(vnode_t *file) {
    kfree(file->internal);
}

static void fat32_vnode_init(vnode_t *vnode, const char *name) {
    vfs_vnode_init(vnode, name);
    vnode->v_ops = &fat32_vop;
    vnode->f_ops = &fat32_fop;
}

static void fat32_create_file_vnode(vnode_t *dir_node, fat32_de_t *dir, fat32_file_t *file) {
    vnode_t *new = kmalloc(sizeof(vnode_t));
    char name[MAX_NAME_SIZE];
    char extention[4];
    uint16_t len = 0;
    while (dir->p1.file_name[len] != (char)32 && len < 8)
        len++;
    strncpy(name, dir->p1.file_name, len);
    strncpy(extention, dir->p1.extention, 3);
    /* Change to lower case */
    for (int i = 0; i < strlen(name); i++) {
        if (*(name + i) >= 65 && *(name + i) <= 90)
            *(name + i) += 32;
        if (i < 3 && *(extention + i) >= 65 && *(extention + i) <= 90)
            *(extention + i) += 32;
    }
    strcat(name, ".");
    strcat(name, extention);
    fat32_vnode_init(new, name);
    new->internal = file;
    list_add_tail(&dir_node->node, &new->node);
}

static void fat32_create_file(vnode_t *dir_node, vnode_t **target, const char *file_name) {
    return ;
}

static void fat32_create_dir(vnode_t *dir_node, const char *dir_name) {
    return ;
}

static int fat32_read(struct file *file, void *buf, size_t len) {
    fat32_file_t *f = (fat32_file_t*)file->vnode->internal;
    if (file->f_pos < 0)
        return -1;
    uint8_t sec_count = (file->f_pos / f->byte_per_sec) % f->sec_per_clu;
    uint32_t start_sec = fat32_clus_to_sec(f->head_sec, f->clus_pos,
                                           f->sec_per_clu, sec_count);
    uint16_t pos = file->f_pos % f->byte_per_sec;
    char sd_buf[BLOCK_SIZE];
    if ((file->f_pos + len) > f->size)
        len = f->size - file->f_pos;
    readblock(start_sec, sd_buf);
    for (size_t i = 0; i < len; i++) {
        *((char*)buf + i) = sd_buf[pos];
        if (++pos == f->byte_per_sec) {
            if (++sec_count == f->sec_per_clu) {
                uint32_t next_clus = fat32_get_next_clus(f->clus_pos, f->fat_sec,
                                                         f->byte_per_sec);
                f->clus_pos = next_clus;
                start_sec = fat32_clus_to_sec(f->head_sec, f->clus_pos,
                                              f->sec_per_clu, 0);
                sec_count = 0;
            } else {
                start_sec++;
            }
            readblock(start_sec, sd_buf);
            pos = 0;
        }
    }
    file->f_pos += len;
    return len;
}

static int fat32_write(struct file *file, const void *buf, size_t len) {
    fat32_file_t *f = (fat32_file_t*)file->vnode->internal;
    if (file->f_pos < 0)
        return -1;
    uint8_t sec_count = (file->f_pos / f->byte_per_sec) % f->sec_per_clu;
    uint32_t start_sec = fat32_clus_to_sec(f->head_sec, f->clus_pos,
                                           f->sec_per_clu, sec_count);
    uint16_t pos = file->f_pos % f->byte_per_sec;
    char sd_buf[BLOCK_SIZE];
    size_t count;
    bool_t no_next_clus = false;
    readblock(start_sec, sd_buf);
    for (count = 0; count < len; count++) {
        sd_buf[pos] = *((char*)buf + count);
        if (++pos == f->byte_per_sec) {
            writeblock(start_sec, sd_buf);
            if (++sec_count == f->sec_per_clu) {
                uint32_t next_clus = fat32_get_next_clus(f->clus_pos, f->fat_sec,
                                                         f->byte_per_sec);
                if (!next_clus) {
                    count++;
                    no_next_clus = true;
                    break;
                } else {
                    f->clus_pos = next_clus;
                    start_sec = fat32_clus_to_sec(f->head_sec, f->clus_pos,
                                                  f->sec_per_clu, 0);
                    sec_count = 0;
                }
            } else {
                start_sec++;
            }
            readblock(start_sec, sd_buf);
            pos = 0;
        }
    }
    writeblock(start_sec, sd_buf);
    if ((file->f_pos + count) > f->size) {
        f->size += count - (f->size - file->f_pos);
        fat32_update_file_size(f);
    }
    if (no_next_clus == false) {
        file->f_pos += count;
    } else {
        file->f_pos = -1;
    }
    return count;
}

int fat_set_mount(mount_t *mount, const char *device) {
    if (!strcmp(device, "sd")) {
        char buf[BLOCK_SIZE];
        mbr_pe_t mbr;
        fat32_bs_t bs;
        fat32_de_t dir;
        /* First partition */
        readblock(0, buf);
        memcpy(&mbr, &buf[446], sizeof(mbr_pe_t));
        readblock(mbr.sector_addr, buf);
        memcpy(&bs.p1, &buf[11], sizeof(fat_bs_p1_t));
        memcpy(&bs.p2, &buf[14], sizeof(fat_bs_p2_t));
        memcpy(&bs.p3, &buf[32], sizeof(fat32_bs_p3_t));
        uint32_t fat_sec = mbr.sector_addr + bs.p2.bpb_rsvd_sec_cnt;
        uint32_t head_sec = fat_sec + bs.p2.bpb_num_fats * bs.p3.bpb_fat_sz;
        uint32_t dir_sec = head_sec + bs.p1.bpb_sec_per_clu * (bs.p3.bpb_root_clus - 2);
        /* Set mount */
        char *fs_name = kmalloc(strlen("fat32") + 1);
        strcpy(fs_name, "fat32");
        mount->fs_name = fs_name;
        mount->umount_ops = &fat32_umount_operation;
        mount->root = kmalloc(sizeof(vnode_t));
        fat32_vnode_init(mount->root, "/");
        /* Set content */
        uint8_t sec_count = 0;
        uint16_t file_count = 0;
        readblock(dir_sec, buf);
        while (buf[file_count * 32 + 11]) {
            memcpy(&dir.p1, &buf[file_count * 32], sizeof(fat32_de_p1_t));
            /* Only for file */
            if (dir.p1.attribute == 32 && dir.p1.file_name[0] != 229) {
                memcpy(&dir.p2, &buf[file_count * 32 + 20], sizeof(fat32_de_p2_t));
                fat32_file_t *f = kmalloc(sizeof(fat32_file_t));
                f->fat_sec = fat_sec;
                f->head_sec = head_sec;
                f->dir_sec = dir_sec;
                f->file_num = file_count;
                f->clus_pos = dir.p2.l_clu_num + dir.p2.h_clu_num * 0x10000;
                f->size = dir.p2.size;
                f->sec_per_clu = bs.p1.bpb_sec_per_clu;
                f->byte_per_sec = bs.p1.bpb_byte_per_sec;
                fat32_create_file_vnode(mount->root, &dir, f);
            }
            if (++file_count == (bs.p1.bpb_byte_per_sec / 32)) {
                if (++sec_count == bs.p1.bpb_sec_per_clu) {
                    uint32_t next_clus = fat32_get_next_clus(bs.p3.bpb_root_clus, fat_sec,
                                                             bs.p1.bpb_byte_per_sec);
                    if (!next_clus) {
                        break;
                    } else {
                        bs.p3.bpb_root_clus = next_clus;
                        dir_sec = fat32_clus_to_sec(head_sec, bs.p3.bpb_root_clus,
                                                    bs.p1.bpb_sec_per_clu, 0);
                        sec_count = 0;
                    }
                } else {
                    dir_sec++;
                }
                readblock(dir_sec, buf);
                file_count = 0;
            }
        }
        return 0;
    }
    return -1;
}

void fat_init(void) {
    fat32_vop.create_file = &fat32_create_file;
    fat32_vop.create_dir = &fat32_create_dir;
    fat32_fop.write = &fat32_write;
    fat32_fop.read = &fat32_read;
}
