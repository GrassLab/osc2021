#include "tmpfs.h"
#include "allocator.h"
#include "uart.h"
#include "cpio.h"
#include "error.h"

#define DIR_TYPE 1
#define FILE_TYPE 2
#define DIR_CAP 10

typedef struct{
	char* name;
	int type;
	int capacity;
	int size;
	void* data;
	void* cache;//only for FILE_TYPE 
}Content;

void cacheInit(Content* content){
	if(content->cache)ERROR("cacheInit error!!");
	if(content->capacity==0){
		content->capacity=2;
	}
	content->cache=(void*)dalloc(content->capacity);
	char* src=(char*)(content->data);
	char* dst=(char*)(content->cache);
	for(int i=0;i<content->size;++i){
		dst[i]=src[i];
	}
}

int tmpfs_Write(file* f,const void* buf,unsigned long len){
	vnode* node=f->node;
	Content* content=(Content*)(node->internal);
	if(content->type==FILE_TYPE){
		if(content->cache==0)cacheInit(content);
		char* cache=(char*)(content->cache);

		if(f->f_pos+len > content->capacity){
			char* new_cache=(char*)dalloc((f->f_pos+len)*2);
			for(int i=0;i<content->size;++i)new_cache[i]=cache[i];
			content->capacity=(f->f_pos+len)*2;
			content->cache=new_cache;
			dfree((unsigned long)cache);
			cache=new_cache;
		}

		const char* buffer=(const char*)buf;
		for(int i=0;i<len;++i){
			cache[f->f_pos]=buffer[i];
			f->f_pos++;
		}

		if(content->size < f->f_pos){
			content->size=f->f_pos;
		}

		return len;
	}else{
		return 0;
	}
}

int tmpfs_Read(file* f,void* buf,unsigned long len){
	vnode* node=f->node;
	Content* content=(Content*)(node->internal);
	int size=content->size;
	if(content->type==FILE_TYPE){
		if(content->cache==0)cacheInit(content);
		char* cache=(char*)(content->cache);

		char* buffer=(char*)buf;
		int ret=0;
		for(int i=f->f_pos;i<size;++i){
			if(ret<len){
				buffer[ret++]=cache[i];
			}else{
				break;
			}
		}

		f->f_pos+=ret;
		return ret;
	}else if(content->type==DIR_TYPE){
		if(f->f_pos>=size)return 0;
		char* buffer=(char*)(buf);
		vnode* child=((vnode**)(content->data))[f->f_pos++];
		content=(Content*)(child->internal);
		int ret=0;
		for(int i=0;i<len;++i){
			if(!content->name[i])break;
			buffer[i]=content->name[i];
			ret++;
		}
		return ret;
	}else{
		return 0;
	}
}

void tmpfs_Dump(vnode* cur,int level){
	Content* content=(Content*)(cur->internal);

	for(int i=0;i<level;++i)uart_printf("\t");

	if(content->type==DIR_TYPE){
		uart_printf("%s\n",content->name);
		vnode** childs=(vnode**)(content->data);
		for(int i=0;i<content->size;++i){
			tmpfs_Dump(childs[i],level+1);
		}
	}else if(content->type==FILE_TYPE){
		uart_printf("%s (%d bytes)\n",content->name,content->size);
	}
}

int tmpfs_Creat(vnode* dir_node,vnode** target,const char* component_name){
	Content* content=(Content*)(dir_node->internal);
	if(content->type!=DIR_TYPE){
		uart_printf("tmpfs_Creat error!!\n");
		while(1){}
	}
	vnode** childs=(vnode**)content->data;
	
	int idx=-1;
	if(content->capacity>content->size){
		idx=content->size++;
	}else{
		uart_printf("tmpfs_Creat: space is not enough!!\n");
		while(1){}
		//TODO: move to larger array
	}
	
	vnode* new_node=(vnode*)dalloc(sizeof(vnode));
	new_node->mnt=dir_node->mnt;
	new_node->v_ops=dir_node->v_ops;
	new_node->f_ops=dir_node->f_ops;
	new_node->internal=(Content*)dalloc(sizeof(Content));

	content=(Content*)new_node->internal;
	content->name=(char*)dalloc(PREFIX_LEN);
	slashIgnore(component_name,content->name,PREFIX_LEN);
	content->type=FILE_TYPE;
	content->capacity=0;
	content->size=0;
	content->data=0;
	content->cache=0;

	childs[idx]=new_node;
	if(target){
		*target=new_node;
	}
	return idx;
}

int tmpfs_Lookup(vnode* dir_node,vnode** target,const char* component_name){
	Content* content=(Content*)(dir_node->internal);
	if(content->type!=DIR_TYPE){
		uart_printf("tmpfs_Lookup error!!\n");
		while(1){}
	}
	vnode** childs=(vnode**)(content->data);

	for(int i=0;i<content->size;++i){
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

int tmpfs_nodeInit(mount* mnt,vnode* root){
	root->mnt=mnt;
	root->v_ops=(vnode_operations*)dalloc(sizeof(vnode_operations));
	root->v_ops->lookup=tmpfs_Lookup;
	root->v_ops->create=tmpfs_Creat;
	root->f_ops=(file_operations*)dalloc(sizeof(file_operations));
	root->f_ops->write=tmpfs_Write;
	root->f_ops->read=tmpfs_Read;
	root->internal=(void*)dalloc(sizeof(Content));

	Content* content=(Content*)(root->internal);
	content->name=0;
	content->type=DIR_TYPE;
	content->capacity=DIR_CAP;
	content->size=0;
	content->data=(void*)dalloc(DIR_CAP*8);

	void* f=fbaseGet();
	unsigned long size;
	while(1){//build tree
		const char* fname=fnameGet(f,&size);
		char* fdata=fdataGet(f,&size);
		int fmode=fmodeGet(f);
		if(strcmp(fname,"TRAILER!!!")==0)break;

		//insert file from root
		vnode* dir_node=root;
		content=(Content*)(dir_node->internal);
		vnode** target=(vnode**)(content->data);
		while(1){//iterative search
			char prefix[PREFIX_LEN];
			fname=slashIgnore(fname,prefix,PREFIX_LEN);
			int idx=tmpfs_Lookup(dir_node,0,prefix);
			if(idx>=0){//next level
				dir_node=target[idx];
				content=(Content*)(dir_node->internal);
				target=(vnode**)(content->data);
			}else{//final level
				if(fname!=0){
					uart_printf("tmpfs_nodeInit error!!\n");
					uart_printf("%s\n%s\n",prefix,fname);
					while(1){}
				}

				idx=tmpfs_Creat(dir_node,0,prefix);
				vnode* new_node=target[idx];
				content=(Content*)(new_node->internal);
				if(fmode==1){
					content->type=DIR_TYPE;
					content->capacity=DIR_CAP;
					content->size=0;
					content->data=(void*)dalloc(DIR_CAP*8);
				}else if(fmode==2){
					content->type=FILE_TYPE;
					content->capacity=size;
					content->size=size;
					content->data=fdata;
					content->cache=0;
					//uart_printf("%s,%d\n",content->name,content->size);
				}else{
					uart_printf("unknown file type!!\n");
					while(1){}
				}
				content=(Content*)(root->internal);
				target=(vnode**)(content->data);
				break;
			}
		}

		f=nextfGet(f);
	}

	return 0;
}

int tmpfs_Setup(filesystem* fs,mount* mnt){
	char* name=(char*)dalloc(6);
	//char tmp[]="tmpfs";//raspi bug!!
	name[0]='t';
	name[1]='m';
	name[2]='p';
	name[3]='f';
	name[4]='s';
	name[5]=0;
	fs->name=name;
	fs->setup_mount=tmpfs_Setup;
	mnt->root=(vnode*)dalloc(sizeof(vnode));
	mnt->fs=fs;

	tmpfs_nodeInit(mnt,mnt->root);
	tmpfs_Dump(mnt->root,0);
	return 0;
}
