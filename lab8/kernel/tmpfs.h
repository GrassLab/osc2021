#ifndef TMPFS_H
#define TMPFS_H

#include "vfs.h"

#define MAX_FILES_IN_DIR 	16
#define TMPFS_BUF_SIZE 		4096

// buffer結構 (用於檔案讀寫使用)
struct tmpfs_buf 
{
	int flag;
	unsigned int size;             // 目前buffer的size
	char buffer[TMPFS_BUF_SIZE];   // 最大buffer size
};

// 檔案描述(vnode的internal)
struct tmpfs_entry 
{
	char name[20];   // 名稱
	enum FILE_TYPE type; // 檔案類型
	struct vnode* vnode; // 節點
	struct vnode* parent_vnode; // 父節點
	struct tmpfs_entry* child[MAX_FILES_IN_DIR]; // 資料夾下的檔案
	struct tmpfs_buf* buf;  // buffer (用於檔案讀寫使用)
};

void tmpfs_init();
int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount);
int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
void tmpfs_set_entry(struct tmpfs_entry* entry, const char* component_name, enum FILE_TYPE type, struct vnode* vnode);
int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name, enum FILE_TYPE type);
int tmpfs_write(struct file* file, const void* buf, unsigned int len);
int tmpfs_read(struct file* file, void* buf, unsigned int len);
// for lab6 elective1
int tmpfs_list(struct file* file, void* buf, int index);
// for lab6 elective2
int tmpfs_set_parent(struct vnode* child_node, struct vnode* parent_vnode);

#endif