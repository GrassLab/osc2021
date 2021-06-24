#include "fat32.h"
#include "util.h"
#include "uart.h"
#include "allocator.h"
#include "sdhost.h"

struct fatfs_boot_sector* fat_boot_sector;
struct fatfs_dir_entry* fat_root_dir_entry;
int fat_starting_sector;
int data_starting_sector;
int root_starting_sector;

struct vnode_operations* fatfs_v_ops;
struct file_operations* fatfs_f_ops;

struct dynamic_allocator *dyalloc_fat = 0;

/*
*  fatfs_init  初始化fatfs
*
*  1. 初始化v_node操作(分配記憶體，註冊查找、建立的方法)
*  2. 初始化file操作(分配記憶體，註冊寫入、讀取的方法)
*  3. 呼叫sd卡driver的sd_init進行初始化
*/
void fatfs_init() 
{
	dyalloc_fat = dynamic_allocator_init();
	
	fatfs_v_ops = (struct vnode_operations*)dynamic_alloc(dyalloc_fat, sizeof(struct vnode_operations));
	fatfs_v_ops->lookup = fatfs_lookup;
	fatfs_v_ops->set_parent = fatfs_set_parent;
	
	fatfs_f_ops = (struct file_operations*)dynamic_alloc(dyalloc_fat, sizeof(struct file_operations));
	fatfs_f_ops->write = fatfs_write;
	fatfs_f_ops->read = fatfs_read;
	fatfs_f_ops->list = fatfs_list;
	
	sd_init();
}

/*
*	fatfs_setup_mount 實作mount方法(Mount Fat32 file system in SD card)
*
*   required 1-1 Get the FAT32 partition (Fat32 file system is in first partition of sd card)
*   required 1-2 Parse the FAT32’s metadata and set up the mount.
*
*	1. 解析並顯示mbr metadata資訊
*   2. 讀取boot sector
*      - 配置一塊fatfs_boot_sector記憶體，由SD卡讀取一個block，並將內容複製給fatfs_boot_sector
*   3. 計算資料起始sector位址(起始sector + 保留sector數 + boot sector個數 * 一個sector多大)
*   4. 建立根目錄
*      - 計算根目錄起始sector位址
*      - 由SD卡讀取一個block, 建立fatfs_dir_entry實體
*      - 建立根目錄entry及vnode，並將SD卡根目錄entry內容建立為根目錄vnode
*   5. 掛載至檔案系統
*/
int fatfs_setup_mount(struct filesystem* fs, struct mount* mount) 
{
	// 1. 解析並顯示mbr metadata資訊
	struct mbr_partition_entry* entry = parse_mbr_metadata();

	// 2. 讀取boot sector
	//    - 配置一塊fatfs_boot_sector記憶體，由SD卡讀取一個block，並將內容複製給fatfs_boot_sector
	fat_starting_sector = entry->starting_sector;

	char* fat_boot = (char*)dynamic_alloc(dyalloc_fat, BLOCK_SIZE);
	readblock(fat_starting_sector, fat_boot);
	int boot_sector_size = sizeof(struct fatfs_boot_sector);
	fat_boot_sector = (struct fatfs_boot_sector*)dynamic_alloc(dyalloc_fat, boot_sector_size);
	char* src = (char*)fat_boot;
	char* dst = (char*)fat_boot_sector;
	for (int i = 0; i < boot_sector_size; i++)
		dst[i] = src[i];

	dynamic_free(dyalloc_fat, (unsigned long long)fat_boot);

	// 3.計算資料起始sector位址(起始sector + 保留sector數 + boot sector個數 * 一個sector多大)
	data_starting_sector =
	  fat_starting_sector + fat_boot_sector->reserved_sector_count +
	  fat_boot_sector->fat_count * fat_boot_sector->sectors_per_fat_32;
	
	// 4.建立根目錄
	//   - 計算根目錄起始sector位址
	root_starting_sector = get_starting_sector(fat_boot_sector->root_cluster);

	//   - 由SD卡讀取一個block, 建立fatfs_dir_entry實體
	char* fat_root = (char*)dynamic_alloc(dyalloc_fat, BLOCK_SIZE);
	readblock(root_starting_sector, fat_root);
	fat_root_dir_entry = (struct fatfs_dir_entry*)fat_root;

	//   - 建立根目錄entry及vnode，並將SD卡根目錄entry內容建立為根目錄vnode
	struct fatfs_entry* root_entry = (struct fatfs_entry*)dynamic_alloc(dyalloc_fat, sizeof(struct fatfs_entry));
	struct vnode* vnode = (struct vnode*)dynamic_alloc(dyalloc_fat, sizeof(struct vnode));
	vnode->mount = mount;
	vnode->v_ops = fatfs_v_ops;
	vnode->f_ops = fatfs_f_ops;
	vnode->internal = (void*)root_entry;
	root_entry->parent_vnode = 0;
	fatfs_set_entry(root_entry, FILE_DIRECTORY, vnode, fat_boot_sector->root_cluster, 4096);
	fatfs_set_directory(root_entry, fat_root_dir_entry);
	// 5.掛載至檔案系統
	mount->fs = fs;
	mount->root = vnode;
	
	return 1;
}

/*
*	parse_mbr_metadata (解析並顯示MBR資訊)
*
*	1. 讀取MBR
*      - mbr位於sd card第一個sector，使用readblock讀取
*	   - 配置一塊mbr_partition_entry記憶體，並將mbr內容複製給mbr_partition_entry
*   2. 印出[Partition type]、[Partition size]、[Block index]資訊 
*   3. 回傳mbr_partition_entry 
*/
struct mbr_partition_entry* parse_mbr_metadata()
{
	// 1. 讀取MBR 
    // mbr位於sd card第一個sector，使用readblock讀取
	/*
	*  MBR 是由 512 位元組組成，MBR 只佔用了其中的 446 個位元組，
	*  另外 64 個位元組交給了 DPT（Disk Partition Table 硬碟分區表），
	*  最後兩個位元組 “55，AA” 是分區的結束標誌
	*/
	char* mbr = (char*)dynamic_alloc(dyalloc_fat, BLOCK_SIZE);
	readblock(0, mbr);
	if (mbr[510] != 0x55 || mbr[511] != 0xAA) 
	{
		uart_putstr("bad magic in MBR\n");
		return 0;
	}

	// 配置一塊mbr_partition_entry記憶體，並將mbr內容複製給mbr_partition_entry
	int entry_size = sizeof(struct mbr_partition_entry);
	struct mbr_partition_entry* entry = (struct mbr_partition_entry*)dynamic_alloc(dyalloc_fat, entry_size);
	char* src = (char*)mbr;
	char* dst = (char*)entry;
	for (int i = 0; i < entry_size; i++)
		dst[i] = src[MBR_PARTITION_BASE + i];  // 由446開始讀取硬碟分區表
	
	dynamic_free(dyalloc_fat, (unsigned long long)mbr);
	
	// 2. 印出[Partition type]、[Partition size]、[Block index]資訊

	uart_putstr("\n========== FAT32 init ==========\n");
	char buf[16] = {0};
	uart_putstr("Partition type: 0x");
	unsignedlonglongToStrHex(entry->partition_type, buf);
	uart_putstr(buf);
	
	if (entry->partition_type == 0xB) // 代表為32bit FAT
		uart_putstr(" (FAT32 with CHS addressing)");

	uart_putstr("\nPartition size: ");
	unsignedlonglongToStr(entry->sector_count, buf);
	uart_putstr(buf);
	uart_putstr(" (sectors)\n");
	
	uart_putstr("Block index: ");
	unsignedlonglongToStr(entry->starting_sector, buf);
	uart_putstr(buf);
	uart_putstr("\n================================\n\n");
	
	// 3. 回傳mbr_partition_entry 
	return entry;
}

/*
*  fatfs_set_entry  設定節點內容
*
*  fatfs_entry結構，包含name、name_len、starting_cluster、type、vnode、parent_vnode、child、buf屬性
*  1. name及name_len在外部設定
*  2. 設定starting_cluster
*  3. 設定vnode
*  4. 設定type
*  5. parent_vnode外面設定
*  6. 分配buf記憶體，並設定buf內容為空
*  7. 假如節點為目錄，初始化目錄下子檔案
*/
void fatfs_set_entry(struct fatfs_entry* entry, enum FILE_TYPE type, struct vnode* vnode, int starting_cluster, int buf_size) 
{
	entry->starting_cluster = starting_cluster;
	entry->vnode = vnode;
	entry->type = type;
	entry->buf = (struct fatfs_buf*)dynamic_alloc(dyalloc_fat, sizeof(struct fatfs_buf));
	entry->buf->size = buf_size;
	
	for (int i = 0; i < FATFS_BUF_SIZE; i++)
		entry->buf->buffer[i] = '\0';

	if (entry->type == FILE_DIRECTORY) 
	{
		for (int i = 0; i < MAX_FILES_IN_DIR; ++i) 
		{
			entry->child[i] = (struct fatfs_entry*)dynamic_alloc(dyalloc_fat, sizeof(struct fatfs_entry));
			entry->child[i]->name[0] = 0;
			entry->child[i]->type = FILE_NONE;
			entry->child[i]->parent_vnode = vnode;
		}
	}
}

/*
*  fatfs_set_directory  設定子檔案or子資料夾
*
*  傳入fatfs_entry 及 fatfs_dir_entry, 其中fatfs_dir_entry為SD卡的
*
*  使用for迴圈，逐一設定
*  1. 判斷檔名是否有值(去除雜訊)
*     - 取出dir_entry的檔名及副檔名組合出檔名
*     - 建立vnode並設定子節點屬性及內容
*/
void fatfs_set_directory(struct fatfs_entry* entry, struct fatfs_dir_entry* dir_entry) 
{
	for (int i = 0; i < MAX_FILES_IN_DIR; ++i) 
	{		
		// 1. 判斷檔名是否有值(去除雜訊)
		int flag = 0;
		for (int j = 0; j < 8; j++) 
		{
			if ((dir_entry + i)->filename[j] == 0) 
			{
				flag = 1;
				break;
			}
		}

		if ((dir_entry + i)->filename[0] && !flag) 
		{
			// - 取出dir_entry的檔名及副檔名組合出檔名
			// ex. filename = test, extension = txt, name => test.txt
			strncpy(entry->child[i]->name, (char*)((dir_entry + i)->filename), 8);
			unsigned int len = strlen(entry->child[i]->name);
			entry->child[i]->name_len = len;
			
			if ((dir_entry + i)->extension[0]) 
			{
				*(entry->child[i]->name + len) = '.';
				strncpy(entry->child[i]->name + len + 1, (char*)((dir_entry + i)->extension), 3);
			}

			// - 建立vnode
			struct vnode* vnode = (struct vnode*)dynamic_alloc(dyalloc_fat, sizeof(struct vnode));
			vnode->mount = 0;
			vnode->v_ops = entry->vnode->v_ops;
			vnode->f_ops = entry->vnode->f_ops;
			vnode->internal = entry->child[i];
			
			// - 設定子節點內容
			// cluster_low是low 2 bytes of cluster, cluster_high是high 2 byte of cluster，所以cluster_high要 * 4
			// ex. cluster_high = 10, cluster_low = 01, cluster index為1001 = 9 
			int starting_cluster = ((dir_entry + i)->cluster_high << 2) + (dir_entry + i)->cluster_low;
			int buf_size = (dir_entry + i)->file_size;
			fatfs_set_entry(entry->child[i], FILE_REGULAR, vnode, starting_cluster, buf_size);
		}
	}
}

/*
*  get_starting_sector  回傳指定cluster索引的起始位置
*
*  因為由2開始編號所以要減2
*  => (cluster編號 - 2) * 每個cluster多大 + 資料起始位置
*/
int get_starting_sector(int cluster) 
{
	return (cluster - 2) * fat_boot_sector->sectors_per_cluster + data_starting_sector;
}

/*
*  fatfs_lookup  實作lookup方法
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
int fatfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name)
{
	struct fatfs_entry* entry = (struct fatfs_entry*)dir_node->internal;
	
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
		entry = ((struct fatfs_entry*)dir_node->internal)->child[i];
		if (strcmp(entry->name, (char*)component_name)) 
		{
			*target = entry->vnode;
			return 1;
		}
	}
	
	return 0;
}

/*
*  fatfs_set_parent  實作set_parent方法
*
*  傳入子節點，父節點
*
*  1. 取出子節點內容結構
*  2. 設定父節點為傳入的父節點
*/
int fatfs_set_parent(struct vnode* child_node, struct vnode* parent_vnode) 
{
	struct fatfs_entry* entry = (struct fatfs_entry*)child_node->internal;
	entry->parent_vnode = parent_vnode;
	return 1;
}

/*
*  fatfs_list  實作list方法
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
int fatfs_list(struct file* file, void* buf, int index) 
{
	struct fatfs_entry* entry = (struct fatfs_entry*)file->vnode->internal;
	
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
*  fatfs_write  實作write方法
*
*  傳入檔案，buffer，長度
*
*  1. 取出節點內容結構
*  2. 由buf讀取len個byte，寫入節點的buffer內
*  3. 節點檔案的buf大小 = file當前位置(file大小)
*  4. 檢查子檔案or子資料夾，假如檔名=節點名稱，設定檔案大小=buf大小
*  5. 取得對應SD檔案cluster，抓出sector位置，並寫入buf內容
*  6. 回傳寫入的byte數
*/
int fatfs_write(struct file* file, const void* buf, unsigned int len) 
{
	struct fatfs_entry* entry = (struct fatfs_entry*)file->vnode->internal;
	for (unsigned int i = 0; i < len; i++) 
	{
		entry->buf->buffer[file->f_pos++] = ((char*)buf)[i];
		if (entry->buf->size < file->f_pos)
			entry->buf->size = file->f_pos;
	}

	for (int i = 0; i < MAX_FILES_IN_DIR; i++) 
	{
		if (strcmpn((char*)((fat_root_dir_entry + i)->filename), entry->name, entry->name_len))
			(fat_root_dir_entry + i)->file_size = entry->buf->size;
	}
	
	writeblock(root_starting_sector, (char*)fat_root_dir_entry);
	int starting_sector = get_starting_sector(entry->starting_cluster);
	writeblock(starting_sector, entry->buf->buffer);

	return len;
}

/*
*  fatfs_read  實作read方法
*
*  傳入檔案，buffer，長度，由傳入的buf中，寫入len byte到該節點的buf內
*
*  1. 取出節點內容結構
*  2. 取得對應SD檔案cluster，抓出起始sector位置
*  3. 讀取sector內容到buffer
*
*  4. 由節點的buffer內取出len byte內容，傳入buf
*  5. 每讀一byte, read_len讀取長度++
*  6. 假如已讀完節點buffer的內容(讀到buf size了)，跳出
*  7. 回傳讀了幾個byte
*/
int fatfs_read(struct file* file, void* buf, unsigned int len) 
{
	unsigned int read_len = 0;
	struct fatfs_entry* entry = (struct fatfs_entry*)file->vnode->internal;
	int starting_sector = get_starting_sector(entry->starting_cluster);
	
	readblock(starting_sector, entry->buf->buffer);

	for (unsigned int i = 0; i < len; i++) 
	{
		((char*)buf)[i] = entry->buf->buffer[file->f_pos++];
		read_len++;
		
		if (read_len == entry->buf->size)
			break;
	}
	
	return read_len;
}
