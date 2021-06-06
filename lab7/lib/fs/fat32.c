#include <stddef.h>
#include <fs/vfs.h>
#include <mm.h>
#include <string.h>
#include <list.h>
#include <stat.h>
#include <file.h>
#include <asm/errno.h>
#include <sdcard.h>
#include <fs/fat32.h>
#include <printf.h>

static int mount_sdcard(struct mount **mountpoint);

static int fat32_lookup(struct vnode* dir_node, struct vnode **target, const char *component_name);
static int fat32_create(struct vnode* dir_node, const char *component_name);
// static int fat32_unlink(struct vnode* dir_node, const char *component_name);
// static int fat32_mkdir(struct vnode* dir_node, const char *component_name);
// static int fat32_rmdir(struct vnode* dir_node, const char *component_name);

static ssize_t fat32_read(struct file *file, void *buf, size_t len);
static ssize_t fat32_write(struct file *file, const void *buf, size_t len);
static int fat32_fsync(struct file *);

struct filesystem fat32 = {
  .name = "fat32",
  .vnode_mount = &mount_sdcard
};

/*
 * placed in vnode.internal
 * for now we only support 512/32 = 16 entry in a directory
 * a vnode is "cached" means
 * folder: all vnodes children of that node has been constructed
 *   file: the file contents has been loaded into memory
 */
struct fat32_cache {
    unsigned int *FAT;
    unsigned int FAT_index;
    unsigned int FAT_size; /* in bytes */
    unsigned int root_index; /* block index in image */
    void *metadata;
    unsigned int metadata_cluster_index; /* block index offset to root index */
    void *data;
    unsigned int data_cluster_index; /* block index offset to root index */
    unsigned char dirty : 1;
    unsigned char cached : 1;
};

static struct vnode_operations fat32_v_ops = {
   .lookup = &fat32_lookup,
   .create = &fat32_create,
//   .unlink = &fat32_unlink,
//   .mkdir = &fat32_mkdir,
//   .rmdir = &fat32_rmdir
};

static struct file_operations fat32_f_ops = {
  .read = &fat32_read,
  .write = &fat32_write,
  .fsync = &fat32_fsync
};

static struct MBR mbrblock;
static struct FAT32_Bootsector bootsector;

static int find_free_block(struct fat32_cache *cache, int start) {
    int mx = cache->FAT_size / sizeof(int);
    for (int i = start; i < mx; i++) {
        if (cache->FAT[i] == FREE_CLUSTER)
            return i;
    }

    return 0;
}

static int end_of_chain(int n) {
    if (n >= END_OF_CHAIN_THRESHOLD)
        return 1;

    return 0;
}

/* only file can be dirty, folder will be synced while doing mkdir */
static int fat32_fsync(struct file *file) {
    struct fat32_cache *cache = file->vnode->internal;

    if (cache->dirty) {
        struct FAT32_ShortEntry *entry = cache->metadata;

        /* TODO: lock ALL */
        if (file->vnode->size != entry->size) {
            /* update metadata */
            entry->size = file->vnode->size;

            /* metadata write back */
            void *metadata_block = (void *)((uintptr_t)entry & ~(size_t)(SECTOR_SIZE - 1));
            writeblock(cache->root_index + cache->metadata_cluster_index - 2, metadata_block);

            /* file write back */
            int idx = cache->data_cluster_index;
            int remained_size = entry->size;
            int next = 0;

            for (int i = 0; remained_size > 0; i++) {
                writeblock(cache->root_index + idx - 2, (char *)cache->data + i * SECTOR_SIZE);
                remained_size -= SECTOR_SIZE;

                if (end_of_chain(cache->FAT[idx]) && remained_size > 0) {
                    int new_idx = find_free_block(cache, next);
                    if (!new_idx) {
                        return -ENOSPC;
                    }
                    next = new_idx + 1;
                    cache->FAT[idx] = new_idx;
                    cache->FAT[new_idx] = END_OF_CHAIN;
                }
                idx = cache->FAT[idx];
            }

            /* FAT table write back */
            int sectors = (next * sizeof(int) / SECTOR_SIZE) + !!((next * sizeof(int)) % SECTOR_SIZE);
            for (int i = 0; i < sectors; i++) {
                writeblock(cache->FAT_index + i, (char *)cache->FAT + i * SECTOR_SIZE);
            }
        } else {
            /* file write back */
            int idx = cache->data_cluster_index;

            for (int i = 0; !end_of_chain(idx); i++) {
                writeblock(cache->root_index + idx - 2, (char *)cache->data + i * SECTOR_SIZE);
                idx = cache->FAT[idx];
            }
        }
    }

    return 0;
}

static inline int block_to_sector_idx(int block_idx) {
    int size = block_idx * sizeof(int);
    return (size / SECTOR_SIZE) - 1 + !!(size % SECTOR_SIZE);
}

static int fat32_create(struct vnode* dir_node, const char *component_name) {
    struct vnode *v;
    fat32_lookup(dir_node, &v, component_name);

    if (v) {
        return -ENOENT;
    }

    struct fat32_cache *dir_cache = dir_node->internal;
    struct FAT32_ShortEntry *subdir = dir_cache->data;
    struct FAT32_ShortEntry *targetdir = NULL;

    for (int i = 0; i < SECTOR_SIZE / sizeof(struct FAT32_ShortEntry); i++) {
        unsigned char flag = subdir[i].name[0];
        if (flag == 0xE5 || flag == 0) {
            targetdir = &subdir[i];
            int block_idx = find_free_block(dir_cache, 0);
            if (!block_idx) {
                return -ENOSPC;
            }
            targetdir->start = block_idx;
            dir_cache->FAT[block_idx] = END_OF_CHAIN;

            memset(targetdir->name, ' ', 11);
            const char *p = component_name;
            int j;
            for (j = 0; j < 8; j++) {
                if (!p[j] || p[j] == '.') {
                    break;
                }
                targetdir->name[j] = p[j] & 0xdf;
            }
            if (p[j]) {
                j++;
                for (int k = 0; k < 3 && p[j]; k++,j++) {
                    targetdir->ext[k] = p[j] & 0xdf;
                }
            }

            targetdir->size = 0;
            targetdir->attr = ATTR_ARCHIVE;

            /* update FAT table */
            int offset = block_to_sector_idx(block_idx);
            writeblock(dir_cache->FAT_index + offset, (char *)dir_cache->FAT + offset * SECTOR_SIZE);

            /* update subdir data */
            writeblock(dir_cache->root_index + dir_cache->data_cluster_index - 2, subdir);

            break;
        }
    }

    if (!targetdir) {
        return -ENOSPC;
    }

    struct fat32_cache *cache = kcalloc(sizeof(struct fat32_cache));
    cache->cached = 1;
    cache->dirty = 0;
    cache->metadata = targetdir;
    cache->metadata_cluster_index = dir_cache->data_cluster_index; /* since we only permit storing metadata in one sector */
    cache->data_cluster_index = targetdir->start;
    cache->FAT = dir_cache->FAT;
    cache->FAT_size = dir_cache->FAT_size;
    cache->FAT_index = dir_cache->FAT_index;
    cache->root_index = dir_cache->root_index;

    struct vnode *node = kmalloc(sizeof(struct vnode));
    node->mnt = dir_node->mnt;
    node->name = strdup(component_name);
    node->parent = dir_node;
    node->subnodes = LIST_HEAD_INIT(node->subnodes);
    node->f_mode = S_IFREG;
    node->v_ops = &fat32_v_ops;
    node->f_ops = &fat32_f_ops;
    node->size = 0;
    node->capacity = 0;
    node->internal = cache;
    insert_head(&dir_node->subnodes, &node->nodes);

    return 0;
}

static ssize_t fat32_read(struct file *file, void *buf, size_t len) {
    struct fat32_cache *cache = file->vnode->internal;
    ssize_t size = len;

    if (size < 0) {
        return -EOVERFLOW;
    }

    ssize_t mx = file->vnode->size - file->f_pos;
    if (size > mx) {
        size = mx;
    }

    memcpy(buf, (char *)cache->data + file->f_pos, size);
    file->f_pos += size;

    return size;
}

static uint32_t align_up(uint32_t size, int alignment) {
  return (size + alignment - 1) & -alignment;
}

static ssize_t fat32_write(struct file *file, const void *buf, size_t len) {
    struct fat32_cache *cache = file->vnode->internal;
    ssize_t size = len;

    if (size < 0) {
        return -EOVERFLOW;
    }

    /* TODO: since kmalloc can allocate more memory space than user requested,
     * we should use that size as capacity to reduce reallocation */
    size_t total_size = file->f_pos + len;

    if (total_size > file->vnode->capacity) {
        unsigned aligned_size = align_up(total_size, SECTOR_SIZE);
        void *content = kcalloc(aligned_size);
        file->vnode->capacity = aligned_size;

        memcpy(content, cache->data, file->f_pos);
        memcpy((char *)content + file->f_pos, buf, len);
        kfree(cache->data);

        cache->data = content;
    } else {
        memcpy((char *)cache->data + file->f_pos, buf, len);
    }

    if (total_size > file->vnode->size) {
        file->vnode->size = total_size;
    }

    file->f_pos += len;
    cache->dirty = 1;

    return size;
}

static void build_file_cache(struct vnode *node) {
    struct fat32_cache *cache = node->internal;
    unsigned aligned_size = align_up(node->size, SECTOR_SIZE);
    node->capacity = aligned_size;
    cache->data = kcalloc(aligned_size);

    void *tmpbuf = kmalloc(SECTOR_SIZE);
    int index = cache->data_cluster_index;
    unsigned remain_size = node->size;
    char *cpyptr = cache->data;

    while (!end_of_chain(index)) {
        readblock(cache->root_index + index - 2, tmpbuf);
        unsigned int copy_size = remain_size > SECTOR_SIZE ? SECTOR_SIZE : remain_size;
        memcpy(cpyptr, tmpbuf, copy_size);

        index = cache->FAT[index];
        remain_size -= SECTOR_SIZE;
        cpyptr += SECTOR_SIZE;
    }

    kfree(tmpbuf);
}

static void build_dir_cache(struct vnode *dir_node) {
    struct fat32_cache *parent_cache = dir_node->internal;
    struct FAT32_ShortEntry *entry = parent_cache->data;

    for (unsigned i = 0; entry[i].name[0]; i++) {
        /* deleted entry */
        if (entry[i].name[0] == 0xE5) {
            continue;
        }

        /* TODO: handle LFN */
        if (entry[i].attr & ATTR_LONG_NAME == ATTR_LONG_NAME) {
            continue;
        }

        char *name = kmalloc(0x10);
        int cnt = 0;
        for (int j = 0; j < 8; j++) {
            if (entry[i].name[j] == ' ')
                break;

            name[cnt++] = entry[i].name[j];
        }
        if (entry[i].ext[0] != ' ') {
            name[cnt++] = '.';
            for (int j = 0; j < 3; j++) {
                if (entry[i].ext[j] == ' ')
                    break;

                name[cnt++] = entry[i].ext[j];
            }
        }

        name[cnt] = '\0';

        /* won't cache next layer */
        struct fat32_cache *cache = kcalloc(sizeof(struct fat32_cache));
        cache->metadata = &entry[i];
        cache->metadata_cluster_index = parent_cache->data_cluster_index; /* since we only permit storing metadata in one sector */
        cache->data_cluster_index = entry[i].start;
        cache->FAT = parent_cache->FAT;
        cache->FAT_size = parent_cache->FAT_size;
        cache->FAT_index = parent_cache->FAT_index;
        cache->root_index = parent_cache->root_index;
        cache->cached = 0;

        /* load directory data */
        if (entry[i].attr & ATTR_DIRECTORY) {
            cache->data = kmalloc(SECTOR_SIZE);
            readblock(cache->root_index + cache->data_cluster_index - 2, cache->data);
        }

        struct vnode *node = kmalloc(sizeof(struct vnode));
        node->mnt = dir_node->mnt;
        node->name = name;
        node->parent = dir_node;
        node->subnodes = LIST_HEAD_INIT(node->subnodes);
        node->f_mode = entry[i].attr & ATTR_ARCHIVE ? S_IFREG : S_IFDIR;
        node->v_ops = &fat32_v_ops;
        node->f_ops = &fat32_f_ops;
        node->size = entry[i].attr & ATTR_ARCHIVE ? entry[i].size : 0;
        node->capacity = 0; /* since we haven't allocate memory */
        node->internal = cache;
        insert_head(&dir_node->subnodes, &node->nodes);
    }

    parent_cache->cached = 1;
}

static int fat32_lookup(struct vnode* dir_node, struct vnode **target, const char *component_name) {
    struct fat32_cache *dir_cache = dir_node->internal;
    if (!dir_cache->cached) {
        build_dir_cache(dir_node);
    }

    struct list_head *p;
    struct vnode *v;

    *target = NULL;

    list_for_each(p, &dir_node->subnodes) {
        v = list_entry(p, struct vnode, nodes);
        if (!strcasecmp(v->name, component_name)) {
            struct fat32_cache *cache = v->internal;
            if (!cache->cached) {
                if (S_ISDIR(v->f_mode)) {
                    build_dir_cache(v);
                } else {
                    build_file_cache(v);
                }
            }
            *target = v;
        }
    }

    return 0;
}

static void setup_root_cache(struct fat32_cache *cache) {
    readblock(0, &mbrblock);

    unsigned bootsector_idx = mbrblock.partition[0].sector_start;
    readblock(bootsector_idx, &bootsector);

    unsigned FAT_idx = bootsector_idx + bootsector.ReservedSectors;
    cache->FAT_size = bootsector.SectorsPerFat32 * SECTOR_SIZE;
    cache->FAT_index = FAT_idx;

    cache->FAT = kmalloc(cache->FAT_size);

    for (int i = 0; i < bootsector.SectorsPerFat32; i++) {
        readblock(FAT_idx + i, (char *)cache->FAT + i * SECTOR_SIZE);
    }

    unsigned rootdir_idx = FAT_idx + bootsector.NumberOfFatTables * bootsector.SectorsPerFat32;
    cache->root_index = rootdir_idx;
    cache->data_cluster_index = 2;
    cache->data = kmalloc(SECTOR_SIZE);
    readblock(rootdir_idx, cache->data);

    cache->cached = 0;
    cache->dirty = 0;
    cache->metadata = NULL;
    cache->metadata_cluster_index = 0;
}

static int mount_sdcard(struct mount **mountpoint) {
    struct mount *mnt = kmalloc(sizeof(struct mount));
    if (!mnt) {
        return -ENOSPC;
    }

    struct vnode *root = kcalloc(sizeof(struct vnode));
    if (!root) {
        kfree(mnt);
        return -ENOSPC;
    }

    root->name = "/";
    root->parent = root;
    root->nodes = LIST_HEAD_INIT(root->nodes);
    root->subnodes = LIST_HEAD_INIT(root->subnodes);
    root->f_mode = S_IFDIR;
    root->mnt = mnt;
    root->v_ops = &fat32_v_ops;
    root->f_ops = &fat32_f_ops;
    root->internal = kmalloc(sizeof(struct fat32_cache));

    setup_root_cache(root->internal);
    build_dir_cache(root);

    mnt->root = root;
    mnt->fs = &fat32;

    *mountpoint = mnt;
    return 0;
}

void init_fat32() {
  int ret = register_filesystem(&fat32);
  if (ret) {
    panic("failed to register fat32");
  }
}