#include "tmpfs.h"
#include "allocator.h"
#include "uart.h"
#include "cpio.h"

#define DIR_TYPE 1
#define FILE_TYPE 2
#define DIR_CAP 10
#define PREFIX_LEN 50

typedef struct{
	char* name;
	int type;
	int capacity;
	int size;
	void* data;
}Content;

const char* slashIgnore(const char* src,char* dst,int size){
	for(int i=0;i<size;++i){
		if(src[i]==0){
			dst[i]=0;
			return 0;
		}else if(src[i]=='/'){
			dst[i]=0;
			return src+i+1;
		}else{
			dst[i]=src[i];
		}
	}
	uart_printf("slashIgnore error!!");
	while(1){}
	return 0;
}

void tmpfs_Dump(vnode* cur,int level){
	Content* content=(Content*)(cur->internal);
	if(content->type==DIR_TYPE){
		uart_printf("enter %s, %d\n",content->name,content->size);
		vnode** childs=(vnode**)(content->data);
		for(int i=0;i<content->size;++i){
			tmpfs_Dump(childs[i],level+1);
		}
		uart_printf("exit %s\n",content->name);
	}else if(content->type==FILE_TYPE){
		uart_printf("level: %d, name: %s,size: %d\n",level,content->name,content->size);
	}
}

int tmpfs_Creat(vnode* dir_node,vnode** target,const char* component_name){
	Content* content=(Content*)(dir_node->internal);
	if(content->type!=DIR_TYPE){
		uart_printf("tmpfs_Creat error!!\n");
		while(1){}
	}
	
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
	/*
	TODO: new_node->f_ops;
	*/
	new_node->internal=(Content*)dalloc(sizeof(Content));

	content=(Content*)new_node->internal;
	content->name=(char*)dalloc(PREFIX_LEN);
	slashIgnore(component_name,content->name,PREFIX_LEN);
	/*
	update by caller:
		type
		capacity
		size
		data
	*/

	target[idx]=new_node;
	return idx;
}

int tmpfs_Lookup(vnode* dir_node,vnode** target,const char* component_name){
	Content* content=(Content*)(dir_node->internal);
	if(content->type!=DIR_TYPE){
		uart_printf("tmpfs_Lookup error!!\n");
		while(1){}
	}

	for(int i=0;i<content->size;++i){
		vnode* child=target[i];
		Content* child_content=(Content*)(child->internal);
		if(strcmp(child_content->name,component_name)==0){
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
	/*
	TODO: root->f_ops;
	*/
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
			int idx=tmpfs_Lookup(dir_node,target,prefix);
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

				idx=tmpfs_Creat(dir_node,target,prefix);
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
	char tmp[]="tmpfs";
	for(int i=0;i<6;++i)name[i]=tmp[i];
	fs->name=name;
	fs->setup_mount=tmpfs_Setup;
	
	mnt->root=(vnode*)dalloc(sizeof(vnode));
	mnt->fs=fs;

	tmpfs_nodeInit(mnt,mnt->root);
	tmpfs_Dump(mnt->root,0);
	return 0;
}