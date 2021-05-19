#include "../include/tmpfs.h"
#include "../include/initrd.h"
#include "../include/stringUtils.h"
#include "../include/vfs.h"
#include "../include/memAlloc.h"
#include "../include/uart.h"

#define default_component_size 10
#define DIR 0
#define FILE 1

typedef struct _Node{
    char *component_name;
    int type;
    vnode **components; //the child vnodes
    int compo_size;
    int compo_capacity;
    char* content; //only useful when type is FILE
}Node;

char* slashIgnore(char* src, char* dst){
    for(int i = 0; i<50; ++i){
        if(src[i] == '\0'){
            dst[i] = '\0';
            return 0;
        }else if(src[i] == '/'){
            dst[i] = '\0';
            return src+i+1;
        }else{
            dst[i] = src[i];
        }
    }

    uart_printf("slash error \n");
    return 0;

}

void tmpfsfopsGet(file_operations* f_ops){
    f_ops->read = 0;
    f_ops->write = 0;
}

int tmpfsLookup(vnode* root, vnode** target, char* component_name){
    Node* node = (Node*)(root->internal);
    if(node->type != DIR){
        uart_printf("tmpfsLookup error\n");
        while(1){}
    }

    vnode **child_nodes = node->components;
    for(int i = 0; i< node->compo_size; ++i){
        vnode* child = child_nodes[i];
        Node* child_content = child->internal;
        if(compString(child_content->component_name, component_name) == 0){
            return i;
        }
    }

    return -1;
}

int tmpfsCreate(vnode* parent, vnode** target,char* component_name){
    Node* node = (Node*)(parent->internal);
    if(node->type!=DIR){
        uart_printf("create error \n");
        while(1){}
    }
    int idx = node->compo_size++;
    vnode** childs = node->components;

    vnode* new_vnode = (vnode*)my_alloc(sizeof(vnode));
    childs[idx]=new_vnode;
    new_vnode->mount = parent->mount;
    new_vnode->f_ops = parent->f_ops;
    new_vnode->v_ops = parent->v_ops;
    Node* new_internal = (Node*)(my_alloc(sizeof(Node)));
    new_vnode->internal =  new_internal;

    new_internal->component_name = (char*)my_alloc(50);
    slashIgnore(component_name,new_internal->component_name);
    new_internal->type = FILE;
    new_internal->compo_size = 0;
    new_internal->compo_capacity = 0;
    new_internal->content = 0;
    new_internal->components = 0;

    return idx;

}

int tmpNodeInit(mount* mnt, vnode* root){
    root->mount = mnt;
    root->v_ops = (vnode_operations*)my_alloc(sizeof(vnode_operations));
    root->f_ops = (file_operations*)my_alloc(sizeof(file_operations));
    root->v_ops->lookup = tmpfsLookup;
    root->v_ops->create = tmpfsCreate;
//    root->f_ops->write = tmpfsWrite;
 //   root->f_ops->read = tmpfsRead;

    root->internal = (void*)my_alloc(sizeof(Node));
    Node *node = (Node*)(root->internal);
    node->component_name = 0;
    node->type = DIR;
    node->compo_size = 0;
    node->compo_capacity = default_component_size;
    node->content = 0;
    node->components = (vnode**)my_alloc(sizeof(vnode*) * default_component_size);

    unsigned long fsize;//file size stored in cpio
    cpio_t* cpio_addr = 0x8000000;
    while(1){
        const char* fname = fnameGet(cpio_addr);
        char* fdata = fdataGet(cpio_addr,&fsize);
        int mode = fmodeGet(cpio_addr);
        if(compString(fname, "TRAILER!!!") == 0) break;

        vnode* dir_node = root;
        node = (Node*)(dir_node->internal);
        vnode** target = (vnode**)(node->components);
        while(1){
            char prefix[50];
            fname = slashIgnore(fname,prefix);
            int idx = tmpfsLookup(dir_node,0,prefix);

            if(idx >=0){
                dir_node = target[idx];
                node = (Node*)(dir_node->internal);
                target = (vnode**)(node->components);
            }else{
                if(fname!=0){
                    uart_printf("init error\n");
                    while(1){}
                }
                idx = tmpfsCreate(dir_node,0,prefix);
                vnode* new_vnode = target[idx];
                node = (Node*)(new_vnode->internal);
                if(mode == DIR){
                    node->type = DIR;
                    node->compo_capacity = default_component_size;
                    node->compo_size = 0;
                    node->components = (vnode**)(my_alloc(sizeof(vnode*) * default_component_size));
                }else if(mode == FILE){
                    node->type = FILE;
                    node->compo_capacity = 0;
                    node->compo_size = 0;
                    node->components = 0;
                    node->content = fdata;
                }else{
                    uart_printf("unknown type\n");
                    while(1){};
                }
                node = (Node*)(root->internal);
                target = (vnode**)(node->components);
                break;
            }
        }
        cpio_addr = nextfile(cpio_addr);
    }
   return 0;
}

void tmpfsDump(vnode* root,int level){
    Node* node = (Node*)(root->internal);
    for(int i = 0 ;i< level ;++i){
        uart_printf("\t");
    }
    if(node->type==DIR){
        uart_printf("%s\n",node->component_name);
        vnode** childs = node->components;
        for(int i =0;i < node->compo_size;++i){
            tmpfsDump(childs[i],level+1);
        }
    }else if(node->type == FILE){
        uart_printf("%s\n",node->component_name);
    }

}
int tmpfsSetup(filesystem *fs, mount* mnt){
    char* name = my_alloc(6);
    name = "tmpfs";
    fs->name = name;
    fs->setup_mount = tmpfsSetup;
    mnt->root = (vnode*)my_alloc(sizeof(vnode));
    mnt->fs = fs;

    tmpNodeInit(mnt,mnt->root);
    tmpfsDump(mnt->root,0);
    return 0;
}
