#ifndef VFS_H
#define VFS_H

#define O_CREAT 1     // flags use for create file

// 檔案類型，參考助教的圖(Directory、Regular、Device)
enum FILE_TYPE
{ 
	FILE_NONE,
	FILE_DIRECTORY, 
	FILE_REGULAR, 
	FILE_DEVICE 
};

// 節點
struct vnode 
{
	struct mount* mount;             // 屬於哪個mount
	struct vnode_operations* v_ops;  // v_node操作(建立、搜尋)
	struct file_operations* f_ops;   // file操作(讀取、寫入)
	void* internal;                  // 節點內容(各檔案系統實作)
};

// 檔案
struct file 
{
	struct vnode* vnode; // 所屬節點	
	unsigned int f_pos;  // 檔案目前位置(file position)，供讀寫操作使用
	struct file_operations* f_ops;   // file操作(讀取、寫入) (各檔案系統實作)
	int flags;  // O_CREAT
};

// 掛載
struct mount 
{
	struct vnode* root;  	// root節點
	struct filesystem* fs;  // 檔案系統
};

// 檔案系統
struct filesystem 
{
	const char* name;	// 名稱
	int (*setup_mount)(struct filesystem* fs, struct mount* mount);  // 掛載方法(各檔案系統實作)
	struct filesystem* next;  // 指向下一個檔案系統指標
};

// 檔案操作(讀取、寫入)
struct file_operations 
{
	//寫入方法(各檔案系統實作)
	int (*write)(struct file* file, const void* buf, unsigned int len); 
	//讀取方法(各檔案系統實作)
	int (*read)(struct file* file, void* buf, unsigned int len); 
	// list方法(各檔案系統實作) lab6加分1
	int (*list)(struct file* file, void* buf, int index);
};

// v_node操作(查找、建立)
struct vnode_operations 
{
	//查找方法(各檔案系統實作)
	int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
	//建立vnode方法(各檔案系統實作)
	int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name, enum FILE_TYPE type);
	// 設定parent node(各檔案系統實作) lab6加分2
	int (*set_parent)(struct vnode* child_node, struct vnode* parent_node);
};

void vfs_init();
void register_filesystem(struct filesystem* fs);
struct filesystem* search_fs(char* name);
int vfs_find_vnode(struct vnode** target, const char* pathname);  // lab6加分2
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, unsigned int len);
int vfs_read(struct file* file, void* buf, unsigned int len);
int vfs_list(struct file* file, void* buf, int index);
// lab6加分2
int vfs_mkdir(const char* pathname);
int vfs_chdir(const char* pathname);
int vfs_mount(const char* device, const char* mountpoint, const char* filesystem);
int vfs_umount(const char* mountpoint);

#endif