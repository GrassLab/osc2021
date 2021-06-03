#define MAX_PROCENT_NR 16
#define MAX_PROC_BUF   16

struct procent {
    struct procent *rsib;
    struct procent *parent;
    struct procent *child;
    struct vnode *vnode;
    char name[16];
    int type; // FILEENT for file, DIRENT for directory
    // Following members are only useful for file.
    char data_start[MAX_PROC_BUF];
    int size;
    int free;
};


void init_procent_pool();
struct procent *new_procent();
int procfs_switch_write(struct file* file, const void* buf, int len);
int procfs_write(struct file* file, const void* buf, int len);
int procfs_read(struct file* file, void* buf, int len);
int procfs_lookup(struct vnode* dir_node, struct vnode** target,
    const char* component_name);
int procfs_create(struct vnode* dir_node, struct vnode** target,
    const char* component_name, int type);
int procfs_get_parent(struct vnode* dir_node, struct vnode** target);
int procfs_stat(struct vnode *vnode, struct dentry *dent);
int procfs_get_rsib(struct vnode *vnode, struct vnode **target);
int procfs_get_child(struct vnode *vnode, struct vnode **target);
int procfs_setup_mount(struct filesystem* fs,
    struct mount* mount, struct vnode *root);
int init_procfs();