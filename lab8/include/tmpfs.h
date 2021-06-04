#ifndef _TMPFS_H
#define _TMPFS_H
#include "vfs.h"

typedef struct _Node{
    char *component_name;
    int type;
    vnode **components; //the child vnodes
    int compo_size;
    int compo_capacity;
    char* content; //only useful when type is FILE
    unsigned long fsize;
}Node;

int tmpfsSetup(filesystem* fs, mount* mnt);
void tmpfsfopsGet(file_operations* f_ops);
void tmpfsDump(vnode* dir_node,int level);
char* slashIgnore(char* src,char* dst);

#endif
