struct mount {
	struct vnode* root;
	struct filesystem* fs;
};

struct filesystem {
	const char* name;
	int (*setup_mount)(struct filesystem* fs, struct mount* mnt);
};

struct vnode {
	struct mount* mnt;
	struct vnode_operations* v_ops;
	struct file_operations* f_ops;
	void* internal;
};

struct vnode_operations {
	int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
	int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name);
};

struct file {
	struct vnode* node;
	unsigned long f_pos; // The next read/write position of this opened file
	struct file_operations* f_ops;
	int flags;
};

struct file_operations {
	int (*write) (struct file* f, const void* buf, unsigned long len);
	int (*read) (struct file* f, void* buf, unsigned long len);
};

typedef struct mount mount;
typedef struct filesystem filesystem;
typedef struct vnode vnode;
typedef struct vnode_operations vnode_operations;
typedef struct file file;
typedef struct file_operations file_operations;

#define PREFIX_LEN 50
#define O_CREAT 2

const char* slashIgnore(const char* src,char* dst,int size);
file* vfs_open(const char* pathname, int flags);
int vfs_close(file* f);
int vfs_write(file* f,const void* buf,unsigned long len);
int vfs_read(file* f,void* buf,unsigned long len);
void vfs_init(void* setup_mount_f,void* write_f,void* read_f);