#include "tmpfs.h"
#include "util.h"
#include "allocator.h"

struct vnode_operations* tmpfs_v_ops;
struct file_operations* tmpfs_f_ops;

struct dynamic_allocator *dyalloc_fs = 0;

/*
*  tmpfs_init  初始化tmpfs
*
*  1. 初始化v_node操作(分配記憶體，註冊查找、建立的方法)
*  2. 初始化file操作(分配記憶體，註冊寫入、讀取的方法)
*/
void tmpfs_init() 
{
	dyalloc_fs = dynamic_allocator_init();
	
	tmpfs_v_ops = (struct vnode_operations*)dynamic_alloc(dyalloc_fs, sizeof(struct vnode_operations));
	tmpfs_v_ops->lookup = tmpfs_lookup;
	tmpfs_v_ops->create = tmpfs_create;
	tmpfs_v_ops->set_parent = tmpfs_set_parent;
	
	tmpfs_f_ops = (struct file_operations*)dynamic_alloc(dyalloc_fs, sizeof(struct file_operations));
	tmpfs_f_ops->write = tmpfs_write;
	tmpfs_f_ops->read = tmpfs_read;
	tmpfs_f_ops->list = tmpfs_list;
}

/*
*  tmpfs_init  實作mount方法 (掛載檔案系統並建立root node)
*
*  傳入要掛載上去的檔案系統，以及mount物件
*
*  1. 分配root node及root node內容的記憶體
*  2. 初始化root node
*  3. 初始化node內容
*  4. 設定mount屬性
*  5. 回傳成功
*/
int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount) 
{
	/*   mount結構包含 fs 跟 root屬性
	*    root是一個v_node結構，包含 mount、tmpfs_v_ops、tmpfs_f_ops、internal屬性
	*    internal是tmpfs_entry結構，包含name、type、vnode、parent_vnode、child、buf屬性
	*/
	
	// 1. 分配root node及node內容的記憶體
	struct tmpfs_entry* root_entry = (struct tmpfs_entry*)dynamic_alloc(dyalloc_fs, sizeof(struct tmpfs_entry));	
	struct vnode* vnode = (struct vnode*)dynamic_alloc(dyalloc_fs, sizeof(struct vnode));
	// 2. 初始化root node
	vnode->mount = mount;
	vnode->v_ops = tmpfs_v_ops;
	vnode->f_ops = tmpfs_f_ops;
	vnode->internal = (void*)root_entry;
	// 3. 初始化node內容(internal)
	root_entry->parent_vnode = 0;  // 因為掛載系統，所以沒有parent_node，自己就是根目錄
	tmpfs_set_entry(root_entry, "/", FILE_DIRECTORY, vnode);
	// 4. 初始化mount
	mount->fs = fs;
	mount->root = vnode;
	// 5. 回傳成功
	return 1;
}

/*
*  tmpfs_set_entry  設定節點內容 
*  
*  tmpfs_entry結構，包含name、type、vnode、parent_vnode、child、buf屬性
*  1. 設定name
*  2. 設定type
*  3. 設定vnode
*  4. parent_vnode外面設定
*  5. 分配buf記憶體，並設定buf內容為空
*  6. 假如節點為目錄，初始化目錄下子檔案，且初始化buffer目前使用size = TMPFS_BUF_SIZE(表示滿了)
*  7. 假如節點為檔案，buffer目前使用size設為0，
*/
void tmpfs_set_entry(struct tmpfs_entry* entry, const char* component_name, enum FILE_TYPE type, struct vnode* vnode) 
{
	strcpy(entry->name, component_name);
	
	entry->type = type;
	entry->vnode = vnode;
	entry->buf = (struct tmpfs_buf*)dynamic_alloc(dyalloc_fs, sizeof(struct tmpfs_buf));
	for (int i = 0; i < TMPFS_BUF_SIZE; i++) 
		entry->buf->buffer[i] = '\0';

	if (entry->type == FILE_DIRECTORY) 
	{
		for (int i = 0; i < MAX_FILES_IN_DIR; ++i) 
		{
			entry->child[i] = (struct tmpfs_entry*)dynamic_alloc(dyalloc_fs, sizeof(struct tmpfs_entry));
			entry->child[i]->name[0] = 0;
			entry->child[i]->type = FILE_NONE;
			entry->child[i]->parent_vnode = vnode;
		}
		
		entry->buf->size = TMPFS_BUF_SIZE;
	} 
	else if (entry->type == FILE_REGULAR) 
	{
		entry->buf->size = 0;
	}
}

/*
*  tmpfs_create  實作create方法
*
*  傳入目前目錄，(目標node，名稱，type)，在目錄下建立node
*
*  對於目錄下的每一個子節點，尋找檔案類型為無的子節點建立node
*  1. 配置一個vnode記憶體
*  2. 設定vnode屬性
*  3. 呼叫tmpfs_set_entry設定vnode內容(interal)
*  4. 設定target = vnode，回傳1
*  5. 子節點下都已配置(滿了)，回傳-1
*/
int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name, enum FILE_TYPE type) 
{
	for (int i = 0; i < MAX_FILES_IN_DIR; i++) 
	{
		struct tmpfs_entry* entry =((struct tmpfs_entry*)dir_node->internal)->child[i];
		if (entry->type == FILE_NONE) 
		{
			struct vnode* vnode = (struct vnode*)dynamic_alloc(dyalloc_fs, sizeof(struct vnode));
			vnode->mount = 0;
			vnode->v_ops = dir_node->v_ops;
			vnode->f_ops = dir_node->f_ops;
			vnode->internal = entry;
			tmpfs_set_entry(entry, component_name, type, vnode);
			*target = entry->vnode;
			return 1;
		}
	}
	
	return -1;
}

/*
*  tmpfs_lookup  實作lookup方法
*
*  傳入目前目錄，(目標node，搜尋名稱)，搜尋目錄下指定名稱的子目錄或子檔案
*
*  1. 判斷傳入的dir_node檔案類型是否為目錄，假如不是目錄回傳失敗
*  2. 假如搜尋名稱為"."表示為目前目錄，target = 目前節點
*  3. 假如搜尋名稱為".."表示為上一層目錄
*     - 如果沒有父節點，回傳失敗
*	  - 否則target = 父節點
*  4. 搜尋目前目錄下每一個子檔案
*     - 假如名稱 = 搜尋名稱，target = 子檔案的節點，回傳成功
*  5. 都不符合就回傳失敗
*/
int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name) 
{
	struct tmpfs_entry* entry = (struct tmpfs_entry*)dir_node->internal;
	if (entry->type != FILE_DIRECTORY) 
		return 0;

	if (strcmp((char*)component_name, ".")) 
	{
		*target = entry->vnode;
		return 1;
	}
	if (strcmp((char*)component_name, "..")) 
	{
		if (!entry->parent_vnode) 
			return 0;
		*target = entry->parent_vnode;
		return 1;
	}

	for (int i = 0; i < MAX_FILES_IN_DIR; i++) 
	{
		entry = ((struct tmpfs_entry*)dir_node->internal)->child[i];
		if (strcmp(entry->name, (char*)component_name)) 
		{
			*target = entry->vnode;
			return 1;
		}
	}
	
	return 0;
}

/*
*  tmpfs_write  實作write方法
*
*  傳入檔案，buffer，長度
*
*  1. 取出節點內容結構
*  2. 由buf讀取len個byte，寫入節點的buffer內
*  3. 節點檔案的buf大小 = file當前位置(file大小)
*  4. 回傳寫入的byte數
*/
int tmpfs_write(struct file* file, const void* buf, unsigned int len) 
{
	struct tmpfs_entry* entry = (struct tmpfs_entry*)file->vnode->internal;
	for (unsigned int i = 0; i < len; i++) 
	{
		entry->buf->buffer[file->f_pos++] = ((char*)buf)[i];
		if (entry->buf->size < file->f_pos) 
			entry->buf->size = file->f_pos;
	}
	
	return len;
}

/*
*  tmpfs_read  實作read方法
*
*  傳入檔案，buffer，長度，由傳入的buf中，寫入len byte到該節點的buf內
*
*  1. 取出節點內容結構
*  2. 由節點的buffer內取出len byte內容，傳入buf
*  3. 每讀一byte, read_len讀取長度++
*  4. 假如已讀完節點buffer的內容(讀到buf size了)，跳出
*  5. 回傳讀了幾個byte
*/
int tmpfs_read(struct file* file, void* buf, unsigned int len) 
{
	unsigned int read_len = 0;
	struct tmpfs_entry* entry = (struct tmpfs_entry*)file->vnode->internal;
	for (unsigned int i = 0; i < len; i++) 
	{
		((char*)buf)[i] = entry->buf->buffer[file->f_pos++];
		read_len++;
		
		if (read_len == entry->buf->size)
			break;
	}
	
	return read_len;
}

//=======================================================================

/*
*  tmpfs_list  實作list方法
*
*  傳入檔案，buffer，索引，列出資料夾下指定index的檔名，並回傳檔案size
*
*  1. 取出節點內容結構
*  2. 假如不是目錄，回傳失敗
*  3. index超過最大子節點數，回傳失敗
*  4. 資料夾下指定index子節點未分配，回傳失敗
*  5. 複製資料夾下指定index的檔名到buf
*  6. 回傳檔案size
*/
int tmpfs_list(struct file* file, void* buf, int index) 
{
	struct tmpfs_entry* entry = (struct tmpfs_entry*)file->vnode->internal;
	
	if (entry->type != FILE_DIRECTORY) 
		return -1;
	if (index >= MAX_FILES_IN_DIR) 
		return -1;
	if (entry->child[index]->type == FILE_NONE) 
		return 0;
	
	strcpy((char*)buf, entry->child[index]->name);
	return entry->child[index]->buf->size;
}

//=======================================================================

/*
*  tmpfs_list  實作set_parent方法
*
*  傳入子節點，父節點
*
*  1. 取出子節點內容結構
*  2. 設定父節點為傳入的父節點
*/
int tmpfs_set_parent(struct vnode* child_node, struct vnode* parent_vnode) 
{
	struct tmpfs_entry* entry = (struct tmpfs_entry*)child_node->internal;
	entry->parent_vnode = parent_vnode;
	return 1;
}