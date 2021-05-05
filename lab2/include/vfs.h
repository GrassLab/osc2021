struct vnode {
    struct mount* mount;
    struct vnode_operations *v_ops;
    struct file_operations *f_ops;
    void* internal;
    int type;
};

struct file {
    struct vnode *vnode, *child_iter;
    int f_pos; // The next read/write position of this opened file
    struct file_operations *f_ops;
    int flags;
    int ref;
};

struct dentry {
    char name[16];
    int size;
    int type;
};

struct file_operations {
    int (*write) (struct file* file, const void* buf, int len);
    int (*read) (struct file* file, void* buf, int len);
};

struct vnode_operations {
    int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
    int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name, int type);
    int (*get_parent)(struct vnode* dir_node, struct vnode** target);
    int (*stat)(struct vnode *vnode, struct dentry* dent);
    int (*get_rsib)(struct vnode *vnode, struct vnode **target);
    int (*get_child)(struct vnode *vnode, struct vnode **target);
};

struct mount {
    struct vnode* root;
    struct filesystem* fs;
};

struct filesystem {
    const char* name;
    int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

#define MAX_FS_NR 10
#define MAX_MNT_NR 16
#define MAX_VNODE_NR 256
#define MAX_FD_NR 16

#define REG_DIR 0
#define REG_FILE 1
#define O_CREAT (1 << 0)

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount);

void init_mnttab();
struct mount *new_mount();
void init_oftab();
struct file *new_file();
void init_fstab();
int register_filesystem(char *name, unsigned long setup_mount);
void init_root_filesystem();
void init_vnode_pool();
struct vnode *new_vnode();
char *get_next_string(char *str);
struct vnode *get_vnode(const char *pathname, int type);
char *get_file_component(const char *pathname);
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, int len);
int vfs_read(struct file* file, void* buf, int len);