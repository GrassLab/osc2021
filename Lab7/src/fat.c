#include "sd.h"
#include "mm.h"
#include "uart.h"
#include "error.h"
#include "vfs.h"

typedef struct{
	unsigned int partition_beg;
	unsigned int sector_num, sector_size;
	unsigned int table_beg, table_num, table_size;
	unsigned int data_beg;
	unsigned int* table;
	unsigned int* dirty;
}MetaData;

typedef struct{
	unsigned char name1[8];
	unsigned char name2[3];
	unsigned char attr;
	unsigned char other1[8];
	unsigned short id1;
	unsigned char other2[3];
	unsigned short id2;
	unsigned int len;
}Dentry;

typedef struct{
	vnode* parent;
	unsigned char name[13];
	unsigned char type;
	unsigned int id;
	void* cache;
	unsigned int capacity;
	unsigned int len;
	unsigned dirty;
}Content;

static MetaData metadata;

void parseDentry(Dentry* dentry, vnode* node, vnode* parent, int display){
	unsigned char name[13];
	int cnt = 0;
	for(int i = 0; i < 8; i++){
		if(dentry->name1[i] == ' ')break;
		name[cnt++] = dentry->name1[i];
	}

	for(int i = 0; i < 3; i++){
		if(dentry->name2[i] == ' ')break;
		if(i == 0){
			name[cnt++] = '.';
		}
		name[cnt++] = dentry->name2[i];
	}
	name[cnt++] = 0;

	unsigned int id = 0;
	id = dentry->id1;
	id = (id << 16) + dentry->id2;

	unsigned int len = dentry->len;

	unsigned int type = (dentry->attr & 0x10) > 0;

	if(display){
		uart_printf("Parent: %s, File: %s, firstSector: %d, Bytes: %d, Directory: %d\n",((Content*)(parent->internal))->name,name,id,len,type);
	}

	node->mnt = parent->mnt;
	node->v_ops = parent->v_ops;
	node->f_ops = parent->f_ops;
	node->internal = (void*)dalloc(sizeof(Content));
	Content* content = (Content*)(node->internal);
	content->parent = parent;
	for(int i = 0; i < 13; i++){
		content->name[i] = name[i];
	}
	content->type = type;
	content->id = id;
	content->cache = 0;
	content->len = len;
}

unsigned int getChainLen(unsigned int id){
	int ret = 1;
	while(1){
		id = metadata.table[id];
		if(id >= 2 && id <= 0xFFFFFEF){
			ret++;
		}
		else if(id >= 0xFFFFFF8 && id <= 0xFFFFFFF){
			break;
		}
		else{
			ERROR("invaild table entry");
		}
	}
	return ret;
}

unsigned int getChain(unsigned int id, unsigned char** buf){
	unsigned int len = getChainLen(id) * 512;
	*buf = (unsigned char*)falloc(len);

	for(int i = 0; i < len; i += 512){
		readblock(metadata.data_beg + id, (*buf) + i);
		id = metadata.table[id];
	}
	return len;
}

void syncFAT(file* f){
	vnode* node = f->node;
	Content* content = (Content*)(node->internal);
	vnode* parent = content->parent;
	if(content->type == 1){
		ERROR("have not do");
	}
	if(content->cache == 0 || content->dirty == 0)return;
	content->dirty = 0;


	//update table
	int old_blks = getChainLen(content->id);
	int cur_blks = content->len/512;
	if(content->len % 512)cur_blks++;
	if(old_blks > cur_blks){
		ERROR("old_blks > cur_blks");
	}

	int tail = content->id;
	while(metadata.table[tail] < 0xFFFFFF8){
		tail = metadata.table[tail];
	}

	for(int i = 0; old_blks < cur_blks && i < metadata.table_size * 512 / 4; ++i){
		if(metadata.table[i] != 0){
			continue;
		}
		metadata.table[tail] = i;
		metadata.dirty[tail/512] = 1;
		tail = i;
		metadata.table[tail] = 0xFFFFFF8;
		metadata.dirty[tail/512] = i;
		old_blks++;
	}

	for(int i = 0; i < metadata.table_size; ++i){
		if(metadata.dirty[i]){
			writeblock(metadata.table_beg + i, ((char*)(metadata.table)) + i * 512);
			metadata.dirty[i] = 0;
		}
	}

	//update data
	unsigned  char* data = (unsigned char*)(content->cache);
	int cur = content->id;
	for(int i= 0; i < content->len; i += 512){
		if(cur < 2 || cur > 0xFFFFFEF)ERROR("wrong table id!");
		writeblock(metadata.data_beg + cur, data);
		data += 512;
		cur = metadata.table[cur];
	}

	//update parent
	int pos = -1;
	vnode** childs = (vnode**)(((Content*)(parent->internal)) ->cache);
	int child_num = ((Content*)(parent->internal))->len;
	for(int i = 0; i < child_num; ++i){
		vnode* child = childs[i];
		Content* child_content = (Content*)(child->internal);
		if(strcmp(content->name, child_content->name) == 0){
			pos = i;
			break;
		}
	}
	if(pos == -1)ERROR("sync a unknown file!");
	int dirsize = 512/32;
	cur = ((Content*)(parent->internal))->id;
	for(int i = 0; i < pos / dirsize; ++i)cur = metadata.table[cur];
	char* buf = (char*)falloc(512);
	readblock(metadata.data_beg+cur, buf);
	Dentry* dentry = (Dentry*)(buf + (pos % dirsize)*32);
	dentry->len = content->len;
	writeblock(metadata.data_beg+cur, buf);
	ffree((unsigned long)buf);

}

void initCache(Content* content){
	unsigned char* buf;
	unsigned int len = getChain(content->id, &buf);
	content->cache = buf;
	content->capacity = len;
	content->dirty = 0;
}

int writeFAT(file* f, const void* buf, unsigned long len){
	vnode* node = f->node;
	Content* content = (Content*)(node->internal);
	if(content->type != 0)ERROR("invalid file type!");
	if(content->cache == 0)initCache(content);

	char* cache = (char*)(content->cache);
	if(f->f_pos + len > content->capacity){
		char* new_cache = (char*)falloc((f->f_pos + len) * 2);
		for(int i = 0;i < content->len; ++i)new_cache[i] = cache[i];
		content->capacity = (f->f_pos + len) * 2;
		content->cache = new_cache;
		ffree((unsigned long)cache);
		cache=new_cache;
	}

	const char* buffer = (const char*)buf;
	for(int i = 0; i < len; ++i){
		cache[f->f_pos] = buffer[i];
		f->f_pos++;
	}

	if(content->len < f->f_pos){
		content->len = f->f_pos;
	}
	content->dirty=1;

	return len;
}

int readFAT(file* f, void* buf, unsigned long len){
	vnode* node = f->node;
	Content* content = (Content*)(node->internal);
	if(content->type != 0)ERROR("invalid file type!");
	if(content->cache == 0)initCache(content);

	char* cache = (char*)(content->cache);
	char* buffer = (char*)buf;
	int ret = 0;
	for(int i = f->f_pos;i < content->len; ++i){
		if(ret < len){
			buffer[ret++] = cache[i];
		}else{
			break;
		}
	}

	f->f_pos += ret;
	return ret;
}

int createFAT(vnode* dir_node, vnode** target, const char* component_name){
	ERROR("TODO");
	return -1;
}

int lookupFAT(vnode* dir_node, vnode** target, const char* component_name){
	Content* content = (Content*)(dir_node->internal);
	if(content->type!=1)ERROR("not a directory!");
	if(content->cache==0){
		unsigned char* buf;
		unsigned int len = getChain(content->id,&buf);
		vnode** childs = (vnode**)dalloc(len/32*8);
		int cnt = 0;
		for(int i = 0;i < len; i += 32){
			if(buf[i] == 0)break;
			childs[cnt] = (vnode*)dalloc(sizeof(vnode));
			parseDentry((Dentry*)(buf + i), childs[cnt], dir_node, 1);
			cnt++;
		}
		ffree((unsigned long)buf);

		content->cache = childs;
		content->capacity = len/32;
		content->len = cnt;
		content->dirty = 0;
	}

	vnode** childs = (vnode**)(content->cache);
	for(int i = 0; i < content->len; ++i){
		vnode* child = childs[i];
		Content* child_content = (Content*)(child->internal);
		uart_printf("child name %s\n", child_content->name);
		uart_printf("component name %s\n", component_name);
		if(strcmp(child_content->name,component_name) == 0){
			if(target){
				*target = child;
			}
			return i;
		}
	}
	return -1;
}

void parseRoot(vnode* root){
	unsigned char* table = (unsigned char*)falloc(metadata.table_size * 512);
	for(int i = 0; i < metadata.table_size; ++i){
		readblock(metadata.table_beg + i,table + i * 512);
	}

	if(metadata.table != 0)ERROR("dirty table!");

	metadata.table = (unsigned int*)table;
	metadata.dirty = (unsigned int*)falloc(metadata.table_size * 4);
	for(int i = 0; i < metadata.table_size; ++i)metadata.dirty[i] = 0;

	root->v_ops = (vnode_operations*)dalloc(sizeof(vnode_operations));
	root->v_ops->lookup = lookupFAT;
	root->v_ops->create = createFAT;
	root->f_ops = (file_operations*)dalloc(sizeof(file_operations));
	root->f_ops->read = readFAT;
	root->f_ops->write = writeFAT;
	root->f_ops->sync = syncFAT;
	root->internal = (void*)dalloc(sizeof(Content));

	Content* content = (Content*)(root->internal);
	content->name[0] = '.';
	content->name[1] = 0;
	content->type = 1;
	content->id = 2;
	content->cache = 0;
}

void parseFAT32(){
	unsigned char* buf = (unsigned char*)falloc(512);
	readblock(metadata.partition_beg, buf);

	if(buf[510]!=0x55 || buf[511]!=0xaa)ERROR("invalid FAT signature!");
	
	unsigned int sector_size = buf[12];
	sector_size = (sector_size << 8) + buf[11];
	if(sector_size != 512)ERROR("invalid sector_size!");

	unsigned int cluster_size = *(unsigned char*)(buf + 13);
	if(cluster_size != 1)ERROR("invalid cluster_size!");

	unsigned int table_beg = *(unsigned short*)(buf + 14);
	unsigned int table_num = buf[16];

	unsigned int sector_num = buf[20];
	sector_num=(sector_num << 8) + buf[19];
	if(sector_num == 0)sector_num = *(unsigned int*)(buf + 32);
	unsigned int table_size = *(unsigned short*)(buf + 22);
	if(table_size!=0)ERROR("not FAT32!");
	
	table_size = *(unsigned int*)(buf + 36);
	
	metadata.sector_num = sector_num;
	metadata.sector_size = sector_size;
	metadata.table_beg = metadata.partition_beg + table_beg;
	metadata.table_num = table_num;
	metadata.table_size = table_size;
	metadata.data_beg = metadata.table_beg + table_num * table_size - 2;

	ffree((unsigned long)buf);
}

void parseMBR(){
	unsigned char* buf = (unsigned char*)falloc(512);
	readblock(0, buf);

	if(buf[510] != 0x55 || buf[511] != 0xaa)ERROR("invalid MBR signature!");
	if(buf[446] != 0x80)ERROR("invalid partition status!");
	unsigned char* partition_entry = buf + 446;
	unsigned char partition_type = *(unsigned char*)(partition_entry+4);
	if(partition_type != 0xb)ERROR("not FAT32!");
	
	for(int  i = 0; i < 4; ++i){
		buf[i] = partition_entry[8 + i];
		buf[4 + i] = partition_entry[12 + i];
	}
	unsigned int beg = *(unsigned int*)(buf);
	unsigned int num = *(unsigned int*)(buf + 4);
	metadata.partition_beg = beg;

	ffree((unsigned long)buf);
}

int fat_Setup(filesystem* fs, mount* mnt){
	char* name = (char*)dalloc(6);
	name[0]='f';
	name[1]='a';
	name[2]='t';
	name[3]='f';
	name[4]='s';
	name[5]=0;
	fs->name = name;
	fs->setup_mount = fat_Setup;
	mnt->root = (vnode*)dalloc(sizeof(vnode));
	mnt->fs = fs;
	mnt->root->mnt = mnt;
	parseMBR();
	parseFAT32();
	parseRoot(mnt->root);
	return 0;
}