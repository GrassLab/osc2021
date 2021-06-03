#define FAT32_DIRENT_NAME 0
#define DIRENT 1
#define FILEENT 2
#define MAX_FAT32ENT_NR 256

#define ENT_FREE        1
#define ENT_FIRST_VISIT 2

struct fat32_meta {
    int Number_of_Reserved_Sectors;
    int Number_of_FATs;
    int Sectors_Per_FAT;
    unsigned long fat_begin_lba;
    unsigned long cluster_begin_lba;
    unsigned char sectors_per_cluster;
    unsigned long root_dir_first_cluster;
};


struct fat32_ent {
    struct fat32_ent *rsib;
    struct fat32_ent *parent;
    struct fat32_ent *child;
    struct vnode *vnode;
    char name[13]; // 8.3 + '\0'
    int type; // FILEENT for file, DIRENT for directory
    int size;
    int first_cluster;
    int cache_cluster;
    // char *page_cache; // cluster data
    int flag;
};


void init_fat32ent_pool();
struct fat32_ent *new_fat32ent();
unsigned long cluster_to_lba(unsigned long cluster);
int fat_get_num(char *start, int len);
int fat32_insert_dirent(struct fat32_ent *dir, struct fat32_ent *child);
int fat32_get_next_cluster(int cluster);
int fat32_read_pos_clstr_to_buf(char *buf, int f_pos, struct fat32_ent *ent);
int fat32_write(struct file* file, const void* buf, int len);
int fat32_read(struct file* file, void* buf, int len);
int fat32_create(struct vnode* dir_node, struct vnode** target,
    const char* component_name, int type);
int fat32_parse_name(char *buf, char *fat32_dirent);
int fat32_lookup(struct vnode* dir_node, struct vnode** target,
    const char* component_name);
int fat32_get_parent(struct vnode* dir_node, struct vnode** target);
int fat32_stat(struct vnode *vnode, struct dentry *dent);
int fat32_get_rsib(struct vnode *vnode, struct vnode **target);
int fat32_get_child(struct vnode *vnode, struct vnode **target);
int fat32_setup_mount(struct filesystem* fs,
    struct mount* mount, struct vnode *root);
int init_fat32();
int fat32_sync();