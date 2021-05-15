#include "sd.h"
#include "allocator.h"
#include "uart.h"
#include "error.h"
#include "vfs.h"

/*
Note: https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system
*/

typedef struct{
	unsigned int partition_beg;
	unsigned int sector_num,sector_size;
	unsigned int table_beg,table_num,table_size;
	unsigned int data_beg;
	unsigned int* table;
}MetaData;

typedef struct{
	unsigned char name1[8];
	unsigned char name2[3];
	unsigned char attr;
	unsigned char other1[8];
	unsigned short id1;
	unsigned char other2[4];
	unsigned short id2;
	unsigned int len;
}Dentry;

typedef struct{
	unsigned char name[13];
	unsigned char type;//dir:1, file:0
	unsigned int id;
	void* cache;
	unsigned int capacity;
	unsigned int len;
	unsigned int dirty;
}Content;

static MetaData metadata;

void parseDentry(Dentry* dentry,vnode* node,int display){
	//name
	unsigned char name[13];
	int cnt=0;
	for(int i=0;i<8;++i){
		if(dentry->name1[i]==' ')break;
		name[cnt++]=dentry->name1[i];
	}
	name[cnt++]='.';
	for(int i=0;i<3;++i){
		if(dentry->name2[i]==' '){
			if(i==0)cnt--;
			break;
		}
		name[cnt++]=dentry->name2[i];
	}
	name[cnt++]=0;

	//id
	unsigned int id;
	id=dentry->id1;
	id=(id<<16)+dentry->id2;

	//len
	unsigned int len=dentry->len;

	//type
	unsigned int type=(dentry->attr&0x10)>0;

	if(display)uart_printf("%s %d %d %d\n",name,id,len,type);

	Content* content=(Content*)(node->internal);
	for(int i=0;i<13;++i)content->name[i]=name[i];
	content->type=type;
	content->id=id;
	content->cache=0;
	content->len=len;
}

unsigned int getChainLen(unsigned int id){
	int ret=1;
	while(1){
		id=metadata.table[id];
		if(id>=2&&id<=0xFFFFFEF){
			ret++;
		}else if(id>=0xFFFFFF8&&id<=0xFFFFFFF){
			break;
		}else{
			ERROR("invalid table entry!");
		}
	}
	return ret;
}

unsigned int getChain(unsigned int id,unsigned char** buf){
	unsigned int len=getChainLen(id)*512;
	*buf=(unsigned char*)falloc(len);
	for(int i=0;i<len;i+=512){
 		readblock(metadata.data_beg+id,(*buf)+i);

 		id=metadata.table[id];
	}
	return len;
}

void syncFAT(file* f){
	vnode* node=f->node;
	Content* content=(Content*)(node->internal);
	if(content->type==1)ERROR("TODO");
	if(content->cache==0||content->dirty==0)return;
	ERROR("TODO");
}

void initCache(Content* content){
	unsigned char* buf;
	unsigned int len=getChain(content->id,&buf);
	content->cache=buf;
	content->capacity=len;
	content->dirty=0;
}

int writeFAT(file* f,const void* buf,unsigned long len){
	vnode* node=f->node;
	Content* content=(Content*)(node->internal);
	if(content->type!=0)ERROR("invalid file type!");
	if(content->cache==0)initCache(content);

	char* cache=(char*)(content->cache);
	if(f->f_pos+len > content->capacity){
		char* new_cache=(char*)falloc((f->f_pos+len)*2);
		for(int i=0;i<content->len;++i)new_cache[i]=cache[i];
		content->capacity=(f->f_pos+len)*2;
		content->cache=new_cache;
		ffree((unsigned long)cache);
		cache=new_cache;
	}

	const char* buffer=(const char*)buf;
	for(int i=0;i<len;++i){
		cache[f->f_pos]=buffer[i];
		f->f_pos++;
	}

	if(content->len < f->f_pos){
		content->len=f->f_pos;
	}
	content->dirty=1;

	return len;
}

int readFAT(file* f,void* buf,unsigned long len){
	vnode* node=f->node;
	Content* content=(Content*)(node->internal);
	if(content->type!=0)ERROR("invalid file type!");
	if(content->cache==0)initCache(content);

	char* cache=(char*)(content->cache);
	char* buffer=(char*)buf;
	int ret=0;
	for(int i=f->f_pos;i<content->len;++i){
		if(ret<len){
			buffer[ret++]=cache[i];
		}else{
			break;
		}
	}

	f->f_pos+=ret;
	return ret;
}

int createFAT(vnode* dir_node,vnode** target,const char* component_name){
	ERROR("TODO");
	return -1;
}

int lookupFAT(vnode* dir_node,vnode** target,const char* component_name){
	Content* content=(Content*)(dir_node->internal);
	if(content->type!=1)ERROR("not a directory!");
	if(content->cache==0){
		unsigned char* buf;
		unsigned int len=getChain(content->id,&buf);
		vnode** childs=(vnode**)dalloc(len/32*8);
		int cnt=0;
		for(int i=0;i<len;i+=32){
			if(buf[i]==0)break;
			childs[cnt]=(vnode*)dalloc(sizeof(vnode));
			childs[cnt]->mnt=0;
			childs[cnt]->v_ops=dir_node->v_ops;
			childs[cnt]->f_ops=dir_node->f_ops;
			childs[cnt]->internal=(void*)dalloc(sizeof(Content));
			parseDentry((Dentry*)(buf+i),childs[cnt],1);
			cnt++;
		}
		ffree((unsigned long)buf);

		content->cache=childs;
		content->capacity=len/32;
		content->len=cnt;
		content->dirty=0;
	}

	vnode** childs=(vnode**)(content->cache);
	for(int i=0;i<content->len;++i){
		vnode* child=childs[i];
		Content* child_content=(Content*)(child->internal);
		if(strcmp(child_content->name,component_name)==0){
			if(target){
				*target=child;
			}
			return i;
		}
	}
	return -1;
}

void parseRoot(vnode* root){
	unsigned char* table=(unsigned char*)falloc(metadata.table_size*512);
	for(int i=0;i<metadata.table_size;++i){
		readblock(metadata.table_beg+i,table+i*512);
	}
	//for(int i=0;i<50;++i)uart_printf("%d\n",*(unsigned int*)(table+i*4));
	if(metadata.table!=0)ERROR("dirty table!");
	metadata.table=(unsigned int*)table;

	root->v_ops=(vnode_operations*)dalloc(sizeof(vnode_operations));
	root->v_ops->lookup=lookupFAT;
	root->v_ops->create=createFAT;
	root->f_ops=(file_operations*)dalloc(sizeof(file_operations));
	root->f_ops->read=readFAT;
	root->f_ops->write=writeFAT;
	root->f_ops->sync=syncFAT;
	root->internal=(void*)dalloc(sizeof(Content));

	Content* content=(Content*)(root->internal);
	content->name[0]='.';content->name[1]=0;
	content->type=1;
	content->id=2;
	content->cache=0;
}

void parseFAT32(){
	unsigned char* buf=(unsigned char*)falloc(512);
	readblock(metadata.partition_beg,buf);

	if(buf[510]!=0x55||buf[511]!=0xaa)ERROR("invalid FAT signature!");
	unsigned int sector_size=*(unsigned short*)(buf+11);
	if(sector_size!=512)ERROR("invalid sector_size!");
	unsigned int cluster_size=*(unsigned char*)(buf+13);
	if(cluster_size!=1)ERROR("invalid cluster_size!");
	unsigned int table_beg=*(unsigned short*)(buf+14);
	unsigned int table_num=buf[16];
	unsigned int sector_num=*(unsigned short*)(buf+19);
	if(sector_num==0)sector_num=*(unsigned int*)(buf+32);
	unsigned int table_size=*(unsigned short*)(buf+22);//sectors/table
	if(table_size!=0)ERROR("not FAT32!");
	table_size=*(unsigned int*)(buf+36);
	//uart_printf("%d %d %d %d %d\n",sector_size,table_beg,table_num,sector_num,table_size);
	metadata.sector_num=sector_num;
	metadata.sector_size=sector_size;
	metadata.table_beg=metadata.partition_beg+table_beg;
	metadata.table_num=table_num;
	metadata.table_size=table_size;
	metadata.data_beg=metadata.table_beg+table_num*table_size-2;//-2 is important!

	ffree((unsigned long)buf);
}

void parseMBR(){
	unsigned char* buf=(unsigned char*)falloc(512);
	readblock(0,buf);

	if(buf[510]!=0x55||buf[511]!=0xaa)ERROR("invalid MBR signature!");
	if(buf[446]!=0x80)ERROR("invalid partition status!");
	unsigned char* partition_entry=buf+446;
	unsigned int beg=*(unsigned int*)(partition_entry+8);
	unsigned int num=*(unsigned int*)(partition_entry+12);
	//uart_printf("%d %d\n",start,size);
	metadata.partition_beg=beg;

	ffree((unsigned long)buf);
}

int fat_Setup(filesystem* fs,mount* mnt){
	char* name=(char*)dalloc(6);
	name[0]='f';
	name[1]='a';
	name[2]='t';
	name[3]='f';
	name[4]='s';
	name[5]=0;
	fs->name=name;
	fs->setup_mount=fat_Setup;
	mnt->root=(vnode*)dalloc(sizeof(vnode));
	mnt->fs=fs;
	mnt->root->mnt=mnt;
	parseMBR();
	parseFAT32();
	parseRoot(mnt->root);
	return 0;
}