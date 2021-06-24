#include "vfs.h"
#include "tmpfs.h"
#include "allocator.h"
#include "cpio.h"
#include "uart.h"
#include "util.h"
#include "fat32.h"

struct filesystem *fs_head, *fs_tail;
struct mount* rootfs;
struct vnode* current_dir;

struct dynamic_allocator *dyalloc_vfs = 0;

/*
*  vfs_init()
*
*  1. 初始化參數(動態配置，指向檔案系統頭尾的指標)
*  2. 初始化及註冊tmpfs檔案系統
*  3. 掛載tmpfs檔案系統到根目錄
*  4. 將cpio裡面的檔案掛到根目錄上 
*/
void vfs_init() 
{
	// 1.初始化參數(動態配置，指向檔案系統頭尾的指標)
	dyalloc_vfs = dynamic_allocator_init();
	
	fs_head = 0;
	fs_tail = 0;
	
	// 2.初始化及註冊tmpfs檔案系統
	tmpfs_init();
	struct filesystem* tmpfs = (struct filesystem*)dynamic_alloc(dyalloc_vfs, sizeof(struct filesystem));
	tmpfs->name = "tmpfs";
	tmpfs->setup_mount = tmpfs_setup_mount;
	register_filesystem(tmpfs);
	
	// lab 7, 初始化及註冊fatfs檔案系統
	fatfs_init();
	struct filesystem* fatfs = (struct filesystem*)dynamic_alloc(dyalloc_vfs, sizeof(struct filesystem));
	fatfs->name = "fatfs";
	fatfs->setup_mount = fatfs_setup_mount;
	register_filesystem(fatfs);
	
	// 3.掛載tmpfs檔案系統到根目錄
	rootfs = (struct mount*)dynamic_alloc(dyalloc_vfs, sizeof(struct mount));
	struct filesystem* fs = search_fs("tmpfs");
	if (fs == 0) 
		return;
	
	fs->setup_mount(fs, rootfs);
	current_dir = rootfs->root;

	// 4.將cpio裡面的檔案掛到根目錄上 
	cpio_populate_rootfs();
}

/*
*  register_filesystem()  註冊至filesystem list
*
*  1. 假如為空，直接放入，頭尾指向新加入fs
*  2. 假如不為空，往後加在尾端的next，尾端指向新加入fs
*/
void register_filesystem(struct filesystem* fs) 
{
	if (fs_head == 0) 
	{
		fs_head = fs;
		fs_head->next = 0;
		fs_tail = fs_head;
	} 
	else 
	{
		fs_tail->next = fs;
		fs_tail = fs_tail->next;
	}
}

/*
*  search_fs  在檔案系統List尋找指定檔案系統
*
*  1. 從頭(head)到尾(tail)搜尋
*  2. 假如檔案系統名稱 = 輸入名稱，回傳
*  3. 找不到return 0
*/
struct filesystem* search_fs(char* name) 
{
	for (struct filesystem* fs = fs_head; fs != 0; fs = fs->next) 
	{
		if (strcmp((char*)fs->name, name)) 
			return fs;
	}
	
	return 0;
}

/*
*  vfs_find_vnode  遞迴尋找指定路徑是否存在，找到後設定為target並回傳1，否則回傳0
*
*  1. 搜尋路徑 = "/"，設定target為root根目錄，回傳1
*  2. 設定當前目錄，假如開頭為 / 為絕對路徑，須由根目錄開始查找，否則為相對路徑
*  3. 取得component name, ex. /mnt => mnt
*  4. 搜尋component name是否存在
*     - 沒有找到回傳失敗
*     - 有找到重新設定dir及target, 並往下取出下一層compnent name繼續判斷
*
*  舉例，傳入 /mnt/device，先搜尋/下mnt是否存在，再搜尋/mnt下device是否存在
*/
int vfs_find_vnode(struct vnode** target, const char* pathname) 
{
	// 1. 搜尋名稱 = "/"，設定target為root根目錄，回傳1
	if (strcmp((char*)pathname, "/")) 
	{
		*target = rootfs->root;
		return 1;
	}
	
	// 2. 設定當前目錄，假如開頭為 / 為絕對路徑，須由根目錄開始查找，否則為相對路徑
	struct vnode* dir = current_dir;
	if (pathname[0] == '/') 
		dir = rootfs->root;

    // 3. 取得component name, ex. /mnt => mnt
	char* component_name = strtok((char*)pathname, '/');  
		
	// 4. 搜尋component name是否存在
	//    - 沒有找到回傳失敗
	//    - 有找到重新設定dir及target, 並往下取出下一層compnent name繼續判斷
	while (component_name && *component_name != '\0') 
	{
		int found = dir->v_ops->lookup(dir, target, component_name);
		
		if (!found)
			return 0;
		
		if ((*target)->mount) 
			*target = (*target)->mount->root;
		dir = *target;
		
		component_name = strtok(0, '/');
	}
	
	return 1;
}

/*
*  vfs_open  開啟or建立指定路徑檔案並回傳
* 
*  此註釋來自助教，target:目標node，fd: file descriptor
*  1. 由root搜尋指定路徑檔案
*  2. 假如找到指定路徑檔案，建立file descriptor
*  3. 假如輸入的flage是O_CREAT，則建立新檔案
*
*  dir = 目前資料夾, target = 目標node, fd = file descriptor
*  1. 判斷檔名之前的路徑是否存在(pathname_)
*     ex. (1)pathname: /mnt       -> pathname_: "\0",  filename: mnt
*         (2)pathname: /mnt/a.txt -> pathname_: /mnt,  filename: a.txt
*		  (3)pathname: file1      -> pathname_: file1, filename: NULL
*  2. 判斷檔案是否存在 (filename)
*     - 輸入的flage是O_CREAT且找不到: 建立新vnode, 及建立file
*     - 輸入的flage不是O_CREAT且找到: 建立file
*
*/
struct file* vfs_open(const char* pathname, int flags) 
{	
	// 1. Lookup pathname from the root vnode.
	// 2. Create a new file descriptor for this vnode if found.
	// 3. Create a new file if O_CREAT is specified in flags.
	
	struct vnode* dir = current_dir;
	struct vnode* target = 0;
	struct file* fd = 0;
	
	// 1. 判斷檔名之前的路徑是否存在(pathname_)
	// (1)pathname: /mnt       -> pathname_: "\0",  filename: mnt
	// (2)pathname: /mnt/a.txt -> pathname_: /mnt,  filename: a.txt
	// (3)pathname: file1      -> pathname_: file1, filename: NULL
	char* pathname_ = (char*)dynamic_alloc(dyalloc_vfs, strlen((char*)pathname) + 1);
	strcpy(pathname_, pathname);
	
	char* filename = split_last(pathname_, '/');
	
	if (*pathname_ == '\0' && pathname[0] == '/')
		dir = rootfs->root;
	
	if (filename != 0) 
	{
		int prefix_found = vfs_find_vnode(&dir, pathname_);
		// ex. given pathname /abc/zxc/file1, but /abc/zxc not found
		if (!prefix_found) 
			return 0;
	} 
	else
		filename = (char*)pathname_; 
	
	// 2. 判斷檔案是否存在 (filename)
    //    - 輸入的flage是O_CREAT且找不到: 建立新vnode, 及建立file
    //    - 輸入的flage不是O_CREAT且找到: 建立file
	int file_found = dir->v_ops->lookup(dir, &target, filename);

	if (flags == O_CREAT) 
	{
		if (!file_found) 
		{
			dir->v_ops->create(dir, &target, filename, FILE_REGULAR);
			fd = (struct file*)dynamic_alloc(dyalloc_vfs, sizeof(struct file));
			fd->vnode = target;
			fd->f_ops = target->f_ops;
			fd->f_pos = 0;
		}
	} 
	else 
	{		
		if (file_found) 
		{			
			if (target->mount) 
				target = target->mount->root;
			
			fd = (struct file*)dynamic_alloc(dyalloc_vfs, sizeof(struct file));
			fd->vnode = target;
			fd->f_ops = target->f_ops;
			fd->f_pos = 0;
		} 
	}

	return fd;
}

/*
*  vfs_close  關閉(釋放file descriptor)
*
*  釋放輸入的file descriptor
*/
int vfs_close(struct file* file) 
{
	// 1. release the file descriptor
	dynamic_free(dyalloc_vfs, (unsigned long)file);
	return 1;
}

/*
*  vfs_write  寫入
*
*  此註釋來自助教
*  1. 由buf寫入len byte到開啟的檔案
*  2. 回傳寫入的size，或發生錯誤的錯誤代碼
*/
int vfs_write(struct file* file, const void* buf, unsigned int len) 
{
	// 1. write len byte from buf to the opened file.
	// 2. return written size or error code if an error occurs.
	return file->f_ops->write(file, buf, len);
}

/*
*  vfs_read  讀取
*
*  此註釋來自助教
*  1. 由開啟的檔案讀取len byte到buf中
*  2. 回傳讀取的size，或發生錯誤的錯誤代碼
*/
int vfs_read(struct file* file, void* buf, unsigned int len) 
{
	// 1. read min(len, readable file data size) byte to buf from the opened file.
	// 2. return read size or error code if an error occurs.
	return file->f_ops->read(file, buf, len);
}

//=======================================================================

/*
*  vfs_list 列出資料夾下檔案 
*
*  傳入file，buf(檔名)，index(第幾個) 
*  列出指定資料夾下第幾個檔案，名稱放入buf，回傳檔案size
*/
int vfs_list(struct file* file, void* buf, int index) 
{
	return file->f_ops->list(file, buf, index);
}

//=======================================================================

/*
*  vfs_mkdir  建立目錄
*
*  傳入目錄名稱
*
*  1. 如vfs_open，先判斷指定資料夾之前的路徑是否存在，不存在回傳0
*  2. 再判斷該資料夾是否存在，假如已存在回傳0
*  3. 建立一個新目錄，回傳建立結果
*
*  舉例，傳入/mnt/dir1，先判斷/mnt是否存在，再判斷/mnt下是否有dir1，若無則建立新目錄
*/
int vfs_mkdir(const char* pathname) 
{
	// 1. 先判斷指定資料夾之前的路徑是否存在，不存在回傳0
	struct vnode* dir = current_dir;
	struct vnode* target = 0;

	char* pathname_ = (char*)dynamic_alloc(dyalloc_vfs, strlen((char*)pathname) + 1);
	strcpy(pathname_, pathname);
	// pathname: /mnt      -> pathname_: "\0",  dirname: mnt
	// pathname: /mnt/dir1 -> pathname_: /mnt,  dirname: dir1
	// pathname: dir1      -> pathname_: dir1,  dirname: NULL
	char* dirname = split_last(pathname_, '/');
	if (*pathname_ == '\0' && pathname[0] == '/')
		dir = rootfs->root;

	if (dirname != 0) 
	{
		int prefix_found = vfs_find_vnode(&dir, pathname_);
		// e.g., given pathname /abc/zxc/file1, but /abc/zxc not found
		if (!prefix_found)
		  return 0;
	} 
	else 
		dirname = pathname_;

	// 2. 再判斷該資料夾是否存在，假如已存在回傳0
	int file_found = dir->v_ops->lookup(dir, &target, dirname);
	if (file_found)
		return 0;

	// 3. 建立一個新目錄，回傳建立結果
	int status = dir->v_ops->create(dir, &target, dirname, FILE_DIRECTORY);
	return status;
}

/*
*  vfs_chdir  改變當前目錄
*
*  傳入新目錄名稱
*
*  1. 尋找目錄名稱是否存在，不存在回傳0
*  2. 設置目前目錄 = target
*/
int vfs_chdir(const char* pathname) 
{
	// 1. 尋找目錄名稱是否存在，不存在回傳0
	struct vnode* target = 0;
	int dir_found = vfs_find_vnode(&target, pathname);
	if (!dir_found)
		return 0;

	// 2. 設置目前目錄 = target
	current_dir = target;
	
	return 1;
}

/*
*  vfs_mount  掛載目錄到某個檔案系統上
*
*  傳入device, 掛載目錄名稱，掛載在甚麼檔案系統上
*
*  1. 假如未找到掛載目錄，回傳0 或 假如目錄已掛載，回傳0
*  2. 掛載tmpfs檔案系統到根目錄
*  3. 設定目錄的mount為當前檔案系統，目錄的parent為目前檔案系統的根目錄
*/
int vfs_mount(const char* device, const char* mountpoint, const char* filesystem) 
{
	// 1. 假如未找到掛載目錄，回傳0 或 假如目錄已掛載，回傳0
	struct vnode* target = 0;
	
	int dir_found = vfs_find_vnode(&target, mountpoint);
	if (!dir_found)
		return 0;
	if (target->mount)
		return 0;
	
	// 2. 掛載tmpfs檔案系統到根目錄
	struct mount* mountfs = (struct mount*)dynamic_alloc(dyalloc_vfs, sizeof(struct mount));
	struct filesystem* fs = search_fs((char*)filesystem);
	fs->setup_mount(fs, mountfs);
		
	// 3. 設定目錄的mount為當前檔案系統，目錄的parent為目前檔案系統的根目錄
	target->mount = mountfs;
	mountfs->root->v_ops->set_parent(mountfs->root, target);
	return 1;
}

/*
*  vfs_umount  卸載目錄
*
*  傳入掛載目錄名稱
*
*  1. 如vfs_open，先判斷掛載資料夾之前的路徑是否存在，不存在回傳0
*  2. 判斷掛載資料夾是否存在，不存在回傳0
*  3. 假如資料夾沒有掛載在任何系統上，回傳0
*  4. 設定目錄的mount(=0)，回傳1
*/
int vfs_umount(const char* mountpoint) 
{
	// 1. 如vfs_open，先判斷掛載資料夾之前的路徑是否存在，不存在回傳0
	struct vnode* dir = current_dir;
	struct vnode* target = 0;

	char* pathname_ = (char*)dynamic_alloc(dyalloc_vfs, strlen((char*)mountpoint) + 1);
	strcpy(pathname_, mountpoint);
	// pathname: /mnt      -> pathname_: "\0",  dirname: mnt
	// pathname: /mnt/dir1 -> pathname_: /mnt,  dirname: dir1
	// pathname: dir1      -> pathname_: dir1,  dirname: NULL
	char* dirname = split_last(pathname_, '/');
	if (*pathname_ == '\0' && mountpoint[0] == '/')
		dir = rootfs->root;

	if (dirname != 0) 
	{
		int prefix_found = vfs_find_vnode(&dir, pathname_);
		// e.g., given pathname /abc/zxc/file1, but /abc/zxc not found
		if (!prefix_found)
		  return 0;
	} 
	else 
		dirname = pathname_;

    // 2. 判斷掛載資料夾是否存在，不存在回傳0
	int file_found = dir->v_ops->lookup(dir, &target, dirname);
	if (!file_found)
		return 0;

    // 3. 假如資料夾沒有掛載在任何系統上，回傳0
	if (!target->mount)
		return 0;

    // 4. 設定目錄的mount(=0)，回傳1
	target->mount = 0;
	return 1;
}