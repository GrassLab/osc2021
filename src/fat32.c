#include "sdhost.h"
#include "io.h"
#include "data_type.h"
#include "string.h"
#include "vfs.h"
#include "mm.h"


#define BOOT_SECTOR_INDEX 2048
#define BLOCK_SIZE 512

// BIOS Parameter Block
// DOS 3.31 BPB
// 25 bytes
struct c_BPB {
    // DOS 2.0 BPB
    // 13 bytes
    unsigned char bytes_per_sector[2];
    unsigned char sectors_pre_cluster;
    unsigned char reserved_cluster[2];
    unsigned char file_allocation_tables_num;
    unsigned char root_directory_entries_maximum_num[2]; // FAT32 should be 0.
    unsigned char total_logical_sectors_2[2]; // FAT32 should be 0.
    unsigned char media_descriptor;
    unsigned char logical_sectors_per_FAT[2]; // FAT32 should be 0.

    // ODS 3.31 BPB
    unsigned char sectors_pre_track[2];
    unsigned char headers_num[2];
    unsigned char hidden_sectors[4];
    unsigned char total_logical_sectors[4];
};

struct BPB {
    u32 bytes_per_sector;
    unsigned char sectors_pre_cluster;
    u32 reserved_cluster;
    unsigned char file_allocation_tables_num;
    u32 root_directory_entries_maximum_num; // FAT32 should be 0.
    u32 total_logical_sectors_2; // FAT32 should be 0.
    unsigned char media_descriptor;
    u32 logical_sectors_per_FAT; // FAT32 should be 0.

    u32 sectors_pre_track;
    u32 headers_num;
    u32 hidden_sectors;
    u32 total_logical_sectors;
};

// FAT32 Extended BIOS Parameter Block
struct c_EBPB {
    struct c_BPB bpb;
    unsigned char logical_sectors_per_FAT[4];
    unsigned char drive_description[2];
    unsigned char version[2];
    unsigned char root_directory_start_cluster_num[4];
    unsigned char fs_info_sector_num[2];
    unsigned char boot_sector_copy_index[2];
    unsigned char reserved[6];
    unsigned char physical_drive_num;
    unsigned char various_purposes_byte;
    unsigned char extended_boot_signature;
    unsigned char volume_id[4];
    unsigned char volume_label[11];
    unsigned char signature[8];
};

struct EBPB {
    struct BPB bpb;
    u32 logical_sectors_per_FAT;
    u32 drive_description;
    u32 version;
    u32 root_directory_start_cluster_num;
    u32 fs_info_sector_num;
    u32 boot_sector_copy_index;
    unsigned char reserved[6];
    unsigned char physical_drive_num;
    unsigned char various_purposes_byte;
    unsigned char extended_boot_signature;
    unsigned char volume_id[4];
    unsigned char volume_label[11];
    unsigned char signature[8];
};

struct c_boot_sector {
    unsigned char jump_instruction[3];
    unsigned char OEM_name[8];
    struct c_EBPB ebpb;
};

struct boot_sector {
    unsigned char jump_instruction[3];
    unsigned char OEM_name[8];
    struct EBPB ebpb;
    u64 FAT_index; // File Allocation Table block index
    u64 dir_index;
    u64 cluster_base;
};

// transfer little endian to big endian
u64 l2bend (unsigned char *src, unsigned int size) {
    u64 value = 0, order = 1;
    for (unsigned int i = 0; i < size; i++) {
        value += src[i] * order;
        order *= 256;
    }
    return value;
}

// transfer big endian to little endian
void b2lend (unsigned char *dst, u64 value, u32 size) {
    u64 remain = value;
    for (int i = 0; i < size && remain; i++) {
        dst[i] = remain % 256;
        remain = remain / 256;
    }
}

void fat32_boot_info (struct boot_sector *header) {
    unsigned char buf[BLOCK_SIZE] = {0};
    // read header sector
    readblock(BOOT_SECTOR_INDEX, buf);

    struct c_boot_sector *c_header = (struct c_boot_sector*)buf;

    // copy data
    memcpy(header->jump_instruction, c_header->jump_instruction, 3);
    memcpy(header->OEM_name, c_header->OEM_name, 8);

    // fill BPB
    struct c_BPB *ptr = &c_header->ebpb.bpb;
    struct BPB *bpb = &header->ebpb.bpb;
    bpb->bytes_per_sector = l2bend(ptr->bytes_per_sector, 2);
    bpb->sectors_pre_cluster = ptr->sectors_pre_cluster;
    bpb->reserved_cluster = l2bend(ptr->reserved_cluster, 2);
    bpb->file_allocation_tables_num = ptr->file_allocation_tables_num;
    bpb->root_directory_entries_maximum_num = 0;
    bpb->total_logical_sectors_2 = 0;
    bpb->media_descriptor = ptr->media_descriptor;
    bpb->logical_sectors_per_FAT = 0;

    // fill EBPB
    struct c_EBPB *cebpb = &c_header->ebpb;
    struct EBPB *ebpb = &header->ebpb;
    ebpb->logical_sectors_per_FAT = l2bend(cebpb->logical_sectors_per_FAT, 4);
    ebpb->drive_description = l2bend(cebpb->drive_description, 2);
    ebpb->version = l2bend(cebpb->version, 2);
    ebpb->root_directory_start_cluster_num =
        l2bend(cebpb->root_directory_start_cluster_num, 4);
    ebpb->fs_info_sector_num = l2bend(cebpb->fs_info_sector_num, 2);
    ebpb->boot_sector_copy_index = l2bend(cebpb->boot_sector_copy_index, 2);
    memcpy(ebpb->reserved, cebpb->reserved, 6);
    ebpb->physical_drive_num = cebpb->physical_drive_num;
    ebpb->various_purposes_byte = cebpb->various_purposes_byte;
    ebpb->extended_boot_signature = cebpb->extended_boot_signature;
    memcpy(ebpb->volume_id, cebpb->volume_id, 4);
    memcpy(ebpb->volume_label, cebpb->volume_label, 11);
    memcpy(ebpb->signature, cebpb->signature, 8);

    // more information
    header->FAT_index = BOOT_SECTOR_INDEX + bpb->reserved_cluster +
        ebpb->logical_sectors_per_FAT;
    header->dir_index = header->FAT_index +
        ebpb->logical_sectors_per_FAT;
    header->cluster_base = header->dir_index - 2;
}

enum DIR_Attr {
    ATTR_READ_ONLY = 0x1,
    ATTR_HIDDEN = 0x02,
    ATTR_SYSTEM = 0x04,
    ATTR_VOLUME_ID = 0x08,
    ATTR_DIRECTORY = 0x10,
    ATTR_ARCHIVE = 0x20,
    ATTR_LONG_FILE_NAME = 0x0f,
};

// SFN
struct directory_entry {
    unsigned char short_file_name[8];
    unsigned char short_file_extension[3];
    unsigned char file_attribute;
    unsigned char reserved;
    unsigned char time_10ms;
    unsigned char time[2];

    unsigned char date[2];
    unsigned char owner_id[2]; // owner ids or last modified time
    unsigned char access_right[2];
    unsigned char last_modified_time[2];
    unsigned char last_modified_date[2];
    unsigned char cluster[2];
    unsigned char size[4]; // size or entry of LFN
};

struct fat32_internal {
    u32 start_cluster, current_cluster;
    u32 FAT_index, FAT_offset;
    u32 dir_start_cluster;
    u32 dir_index, dir_offset;
    u32 pos;
    unsigned char buf[BLOCK_SIZE];
};

#define remove_space(buf, size) \
    for (int i = 0; i < size; i++) \
        if (buf[i] == ' ') buf[i] = '\0'

struct boot_sector boot_info;
struct vnode fat32_root;
struct filesystem fat32_fs;
struct vnode_operations fat32_vop;
struct file_operations fat32_fop;
struct mount fat32_mount;

enum cluster_value {
    CV_EOC = 0xffffff8, // last cluster in file
    CV_EOF = 0xfffffff, // end of file
};

u32 next_cluster (u32 current) {
    unsigned char buf[512];
    unsigned char cnext[4];
    u32 index = current * 4 / BLOCK_SIZE + boot_info.FAT_index;
    u32 offset = current * 4 % BLOCK_SIZE;
    readblock(index, buf);
    memcpy(cnext, &buf[offset], 4);
    return l2bend(cnext, 4);
}

u32 cluster2block_id (u32 cluster) {
    return cluster + boot_info.cluster_base;
}

struct vnode *fat32_new_vnode(char *name, u32 type) {
    struct fat32_internal *inter = m_malloc(sizeof(struct fat32_internal));
    struct vnode *node = m_malloc(sizeof(struct vnode));
    *node = (struct vnode) {
        .mount = &fat32_mount,
        .v_ops = &fat32_vop,
        .f_ops = &fat32_fop,
        .childs = NULL,
        .name = strdup(name),
        .type = type,
        .internal = inter,
    };

    *inter = (struct fat32_internal) {
        .start_cluster = 0,
        .current_cluster = 0,
        .FAT_index = 0,
        .FAT_offset = 0,
    };
    return node;
}

unsigned char is_dir_empty (char *buf) {
    for (int i = 0; i < sizeof(struct directory_entry); i++)
        if (buf[i] != '\0') return 0;
    return 1;
}

// return allocated cluster number
u32 allocate_empty_FAT () {
    unsigned char buf[BLOCK_SIZE];
    u32 allocated_cluster = 0;
    for (u32 id = boot_info.FAT_index;; id++) {
        readblock(id, buf);

        for (u32 i = 0; i < BLOCK_SIZE; i += 4) {
            if (buf[i] == '\0' && buf[i + 1] == '\0' && buf[i + 2] == '\0')
                return allocated_cluster;
            allocated_cluster++;
        }
    }
}

int setup_mount (struct filesystem *fs, struct mount* mount) {
    *mount = fat32_mount;
    return 0;
}


void flush_FAT_entry (u32 cluster, char *buf) {
    u32 FAT_index = cluster * 4 / BLOCK_SIZE + boot_info.FAT_index;
    u32 FAT_offset = cluster * 4 % BLOCK_SIZE;
    unsigned char data[BLOCK_SIZE];

    readblock(FAT_index, data);

    for (int i = 0; i < 4; i++)
        data[FAT_offset + i] = buf[i];

    writeblock(FAT_index, data);
}

void flush_FAT_value (u32 cluster, u32 next_cluster) {
    unsigned char buf[4];
    b2lend(buf, next_cluster, 4);
    flush_FAT_entry(cluster, (char *)buf);
}

void find_empy_dir_entry (u32 start_cluster, u32 *cluster, u32 *offset) {
    u32 pre_cluster = start_cluster;

    char buffer[BLOCK_SIZE];
    for (u32 tmp = start_cluster; tmp != CV_EOC && tmp != CV_EOF;
            tmp = next_cluster(tmp)) {
        u32 id = cluster2block_id(tmp);
        readblock(id, buffer);

        for (int i = 0; i < BLOCK_SIZE; i += sizeof(struct directory_entry)) {
            if (is_dir_empty(&buffer[i])) {
                *cluster = tmp;
                *offset = i;
                return;
            }
        }
        pre_cluster = tmp;
    }

    // directory entry is full. Allocate another cluster.
    start_cluster = allocate_empty_FAT();
    *cluster = start_cluster;
    *offset = 0;
    flush_FAT_value(pre_cluster, start_cluster);
    flush_FAT_value(start_cluster, CV_EOF);
}

void flush_dir_entry (u32 cluster, u32 offset, char *name, int type, u32 file_cluster, u32 size) {
    u32 id = cluster2block_id(cluster);
    unsigned char data[BLOCK_SIZE];
    struct directory_entry *dir;
    readblock(id, data);

    dir = (struct directory_entry *)&data[offset];
    memset((char *)dir->short_file_name, ' ', 8);
    memset((char *)dir->short_file_extension, ' ', 3);
    long dot = strfind(name, '.');
    if (dot == -1) {
        u32 size = strlen(name) < 8 ? strlen(name) : 8;
        memcpy(dir->short_file_name, name, size);
    }
    else {
        u32 size = dot < 8 ? dot : 8;
        memcpy(dir->short_file_name, name, size);
        size = strlen(&name[dot + 1]) < 3 ? strlen(&name[dot + 1]) : 3;
        memcpy(dir->short_file_extension, &name[dot + 1], size);
    }

    if (type == VNODE_DIR)
        dir->file_attribute = ATTR_DIRECTORY;
    else if (type == VNODE_FILE)
        dir->file_attribute = ATTR_ARCHIVE;

    b2lend(dir->cluster, file_cluster, 4);
    b2lend(dir->size, size, 4);

    writeblock(id, data);
    //print_block(id);
}

void clearblock (u32 cluster) {
    u32 id = cluster2block_id(cluster);
    char buffer[BLOCK_SIZE] = {0};
    writeblock(id, buffer);
}

int fat32_vnode_flush (struct vnode *vnode) {
    u32 start_cluster = allocate_empty_FAT();
    clearblock(start_cluster);
    u32 FAT_index = start_cluster * 4 / BLOCK_SIZE + boot_info.FAT_index;
    u32 FAT_offset = start_cluster * 4 % BLOCK_SIZE;

    struct fat32_internal *inter = vnode->internal;
    u32 dir_entry_cluster, dir_offset;
    find_empy_dir_entry(inter->dir_start_cluster, &dir_entry_cluster,
        &dir_offset);

    flush_FAT_value(start_cluster, CV_EOF);

    flush_dir_entry(dir_entry_cluster, dir_offset, vnode->name, vnode->type,
        start_cluster, 0);

    *inter = (struct fat32_internal) {
        .start_cluster = start_cluster,
        .FAT_index = FAT_index,
        .FAT_offset = FAT_offset,
        .dir_index = cluster2block_id(dir_entry_cluster),
        .dir_offset = dir_offset,
    };
    return 0;
}


void parse_folder (struct vnode *folder) {
    if (!strcmp(folder->name, ".") || !strcmp(folder->name, "..")) {
        return;
    }
    struct fat32_internal *inter = (struct fat32_internal *)folder->internal;
    unsigned char *buf = inter->buf;

    for (u32 cluster = inter->start_cluster; cluster != CV_EOC &&
            cluster != CV_EOF;) {
        u32 id = cluster2block_id(cluster);
        readblock(id, buf);

        for (int i = 0; i < 512 / sizeof(struct directory_entry); i++) {
            struct directory_entry *entry = (void *)buf + i * sizeof(struct directory_entry);

            // skip LFN
            if (entry->file_attribute != ATTR_READ_ONLY &&
                entry->file_attribute != ATTR_HIDDEN &&
                entry->file_attribute != ATTR_SYSTEM &&
                entry->file_attribute != ATTR_VOLUME_ID &&
                entry->file_attribute != ATTR_DIRECTORY &&
                entry->file_attribute != ATTR_ARCHIVE)
                continue;

            char file_name[13];
            memset(file_name, '\0', 13);
            memcpy(file_name, entry->short_file_name, 8);
            remove_space(file_name, 8);
            if (entry->short_file_extension[0] != ' ') {
                file_name[strlen(file_name)] = '.';
                memcpy(&file_name[strlen(file_name)], entry->short_file_extension, 3);
                remove_space(file_name, 12);
            }
            // uppercase to lowercase
            for (int j = 0; j < 13; j++)
                if (file_name[j] >= 'A' && file_name[j] <= 'Z')
                    file_name[j] = file_name[j] - 'A' + 'a';

            // skip LFN
            if (file_name[6] == '~' && file_name[7] == '1')
                continue;
            // skip deleted entry
            if (file_name[0] == '\xe5')
                continue;

            // create a new vnode
            unsigned int file_cluster = l2bend(entry->cluster, 2);
            struct vnode *node;
            if (entry->file_attribute == ATTR_DIRECTORY)
                node = fat32_new_vnode(file_name, VNODE_DIR);
            else
                node = fat32_new_vnode(file_name, VNODE_FILE);
            struct vnode_child *child = m_malloc(sizeof(struct vnode_child));
            child->child = node;
            child->next = folder->childs;
            folder->childs = child;
            struct fat32_internal *inter = node->internal;
            inter->start_cluster = file_cluster;
            inter->FAT_index = file_cluster * 4 / BLOCK_SIZE + boot_info.FAT_index;
            inter->FAT_offset = file_cluster * 4 % BLOCK_SIZE;
            inter->dir_index = id;
            inter->dir_offset = sizeof(struct directory_entry) * i;

            //kprintf("%s: ", file_name);
            //kprintf("%d\n", file_cluster);

            // parse next level folder
            if (node->type == VNODE_DIR)
                parse_folder(node);
        }
        cluster = next_cluster(cluster);
    }
}

void parse_fs () {
    struct fat32_internal *inter = m_malloc(sizeof(struct fat32_internal));
    fat32_root = (struct vnode) {
        .mount = &fat32_mount,
        .v_ops = &fat32_vop,
        .f_ops = &fat32_fop,
        .childs = NULL,
        .name = "fat32",
        .type = VNODE_DIR,
        .internal = inter,
    };

    inter->start_cluster = 2;
    parse_folder(&fat32_root);
}

int fat32_lookup (struct vnode* dir_node, struct vnode** target, const char* component_name) {
    if (!strlen(component_name))
        return 0;

    char buffer[256];
    long next_solidus = strfind(component_name, '/');
    if (next_solidus == -1)
        strncopy(buffer, component_name, strlen(component_name) + 1);
    else
        strncopy(buffer, component_name, next_solidus + 1);

    for (struct vnode_child *child = dir_node->childs; child; child = child->next) {
        if (!strcmp(buffer, child->child->name)) {
            *target = child->child;

            /* find next level child node */
            if (next_solidus != -1 && component_name[next_solidus + 1] != '\0') {
                struct vnode *tmp;
                int flag = (*target)->v_ops->lookup(*target, &tmp, &component_name[next_solidus + 1]);
                *target = tmp;
                return flag;
            }
            return 0;
        }
    }
    *target = NULL;
    return -1;
}

int fat32_create (struct vnode* dir_node, struct vnode** target, const char* component_name) {
    long next_solidus = strfind(component_name, '/');
    /* create a new vnode */
    if (next_solidus == -1) {
        *target = fat32_new_vnode((char *)component_name, 0);
    }
    /* create folder recursively */
    else {
        struct vnode *tmp;
        char buffer[256];
        strncopy(buffer, component_name, next_solidus + 1);
        int flag = dir_node->v_ops->lookup(dir_node, &tmp, buffer);

        /* no such directory */
        if (flag) {
            *target = NULL;
            return -1;
        }

        tmp->v_ops->create(tmp, target, &component_name[next_solidus + 1]);
        return 0;
    }

    if (*target) {
        struct vnode_child *list = m_malloc(sizeof(struct vnode_child));
        *list = (struct vnode_child) {
            .child = *target,
            .next = NULL,
        };

        if (dir_node->childs) {
            list->next = dir_node->childs;
            dir_node->childs = list;
        }
        else {
            dir_node->childs = list;
        }
        struct fat32_internal *inter = (*target)->internal;
        struct fat32_internal *dir_inter = dir_node->internal;
        inter->dir_start_cluster = dir_inter->start_cluster;
        //kprintf("start:%d\n", dir_inter->start_cluster);
    }
    return 0;
}

u32 get_file_size (struct fat32_internal *info) {
    unsigned char data[512];
    readblock(info->dir_index, data);
    struct directory_entry *dir =
        (struct directory_entry *)&data[info->dir_offset];
    return l2bend(dir->size, 4);
}

void update_file_size (struct fat32_internal *info, u32 size) {
    unsigned char data[512];
    readblock(info->dir_index, data);
    struct directory_entry *dir =
        (struct directory_entry *)&data[info->dir_offset];
    b2lend(dir->size, size, 4);
    //print_block(info->dir_index);
    writeblock(info->dir_index, data);
    //kprintf("after~~\n");
    //print_block(info->dir_index);
}

int fat32_read (struct file *file, void *buf, u32 len) {
    char *buffer = buf;
    if (!file->internal) {
        file->internal = m_malloc(sizeof(struct fat32_internal));
        memcpy(file->internal, file->vnode->internal,
            sizeof(struct fat32_internal));
    }

    struct fat32_internal *inter = file->internal;

    if (!inter->current_cluster) {
        inter->current_cluster = inter->start_cluster;
        u32 id = cluster2block_id(inter->current_cluster);
        readblock(id, inter->buf);
        inter->pos = 0;
    }
    u32 file_size = get_file_size(inter);
    u32 access_size = len < file_size - file->f_pos ? len :
        file_size - file->f_pos;
    u32 ret_size = 0;
    u32 tmp;

    for (tmp = inter->current_cluster; tmp != CV_EOC && tmp != CV_EOF &&
            ret_size < access_size;) {
        u32 size = BLOCK_SIZE - inter->pos;

        if (access_size - ret_size < size) {
            size = access_size - ret_size;
            memcpy(&buffer[ret_size], &inter->buf[inter->pos], size);
            inter->pos += size;
        }

        else {
            memcpy(&buffer[ret_size], &inter->buf[inter->pos], size);
            tmp = next_cluster(tmp);
            inter->pos = 0;
            if (tmp == CV_EOC || tmp == CV_EOF)
                break;
            u32 id = cluster2block_id(tmp);
            readblock(id, inter->buf);
        }
        file->f_pos += size;
        ret_size += size;
    }
    inter->current_cluster = tmp;
    return access_size;
}

int fat32_write(struct file* file, const void* buf, u32 len) {
    const char *buffer = buf;
    if (!file->internal) {
        file->internal = m_malloc(sizeof(struct fat32_internal));
        memcpy(file->internal, file->vnode->internal,
            sizeof(struct fat32_internal));
    }

    struct fat32_internal *inter = file->internal;

    if (!inter->current_cluster) {
        inter->current_cluster = inter->start_cluster;
        u32 id = cluster2block_id(inter->current_cluster);
        readblock(id, inter->buf);
        inter->pos = 0;
    }

    u32 ret_size = 0;
    u32 pre_cluster, cur_cluster = inter->current_cluster;

    for (; ret_size < len;) {
        u32 size = BLOCK_SIZE - inter->pos;
        u32 id = cluster2block_id(cur_cluster);

        if (len - ret_size < size) {
            size = len - ret_size;
            memcpy(&inter->buf[inter->pos], (char *)&buffer[ret_size], size);
            inter->pos += size;
        }
        // new cluster
        else {
            memcpy(&inter->buf[inter->pos], (char *)&buffer[ret_size], size);
            pre_cluster = cur_cluster;
            inter->pos = 0;
            cur_cluster = next_cluster(cur_cluster);
            if (cur_cluster == CV_EOC || cur_cluster == CV_EOF) {
                cur_cluster = allocate_empty_FAT();
                flush_FAT_value(pre_cluster, cur_cluster);
                flush_FAT_value(cur_cluster, CV_EOF);
            }
        }
        writeblock(id, inter->buf);
        inter->current_cluster = cur_cluster;
        ret_size += size;
    }
    file->f_pos += ret_size;
    update_file_size(inter, file->f_pos);
    //u32 t = get_file_size(inter);
    //kprintf("%d\n", t);
    return ret_size;
}

void fat32_init () {
    fat32_boot_info(&boot_info);

    fat32_mount = (struct mount) {
        .root = &fat32_root,
        .fs = &fat32_fs,
    };

    fat32_fs = (struct filesystem) {
        .name = "fat32",
        .setup_mount = setup_mount,
    };

    fat32_vop.lookup = fat32_lookup;
    fat32_vop.create = fat32_create;
    fat32_vop.flush = fat32_vnode_flush;

    fat32_fop.read = fat32_read;
    fat32_fop.write = fat32_write;

    parse_fs();

    register_filesystem(&fat32_fs);
}
