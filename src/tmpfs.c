#include "tmpfs.h"
#include "vfs.h"
#include "string.h"
#include "system.h"
struct file_operations *tmpfs_file_operations;
struct vnode_operations *tmpfs_vnode_operations;

void tmpfs_set_entry(tmpfs_entry *entry, struct vnode *vnode, const char *component_name){
    entry->vnode = vnode;
    strcpy(entry->name, component_name);
}
int tmpfs_setup_mount(struct filesystem *fs, struct mount *mount){
    mount->fs = fs;
    
    tmpfs_entry* root_entry = (tmpfs_entry*)kmalloc(sizeof(tmpfs_entry));
    struct vnode* vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
    vnode->v_ops = tmpfs_vnode_operations;
    vnode->f_ops = tmpfs_file_operations;
    vnode->internal = (void*)root_entry;
    root_entry->type = FILE_DIRECTORY;
    tmpfs_set_entry(root_entry, vnode, "/");
    for(int i = 0; i < TMPFS_CHILD_MAX; ++i){
        root_entry->list[i] = (tmpfs_entry*)kmalloc(sizeof(tmpfs_entry));
        root_entry->list[i]->type = FILE_NONE;
        root_entry->list[i]->name[0] = 0;
        root_entry->list[i]->parent = root_entry;
    }
    mount->root = vnode;
    return 1;
}

int tmpfs_write(struct file *file, const void *buf, size_t len){
    printf("len : %d\n", len);
    for(int i = 0; i < len; ++i){
        ((tmpfs_entry*)(file->vnode->internal))->buf->buffer[file->f_pos++] = ((char*)buf)[i];
        if(((tmpfs_entry*)(file->vnode->internal))->buf->size < file->f_pos)
            ((tmpfs_entry*)(file->vnode->internal))->buf->size = file->f_pos;
    }
    return len;
}
int tmpfs_read(struct file *file, void *buf, size_t len){
    int res = 0;
    for(int i = 0; i < len; ++i){
        ((char*)buf)[i] =  ((tmpfs_entry*)(file->vnode->internal))->buf->buffer[file->f_pos++];
        ++res;
        if(res == ((tmpfs_entry*)(file->vnode->internal))->buf->size)
            break;
    }
    return res;
}
int tmpfs_list(struct file *file, void *buf, int id){
    if(id >= TMPFS_CHILD_MAX) return -1;
    if(((tmpfs_entry*)(file->vnode->internal))->type == FILE_DIRECTORY){
        if(((tmpfs_entry*)(file->vnode->internal))->list[id]->type != FILE_NONE){
            strcpy((char*)buf, ((tmpfs_entry*)(file->vnode->internal))->list[id]->name);
            return ((tmpfs_entry*)(file->vnode->internal))->list[id]->buf->size;
        }
        else
            return 0;
    }
    return -1;
}
int tmpfs_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name){
    for(int i = 0; i < TMPFS_CHILD_MAX; ++i){
        if(strcmp(((tmpfs_entry*)(dir_node->internal))->list[i]->name, component_name) == 0){
            tmpfs_entry* tmp = ((tmpfs_entry*)(dir_node->internal))->list[i];
            *target = tmp->vnode;
            return 1;
        }
    }
    return -1; // not found
}
int tmpfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name){
    // create file
    for(int i = 0; i < TMPFS_CHILD_MAX; ++i){
        if(((tmpfs_entry*)(dir_node->internal))->list[i]->type == FILE_NONE){
            tmpfs_entry* tar = ((tmpfs_entry*)(dir_node->internal))->list[i];
            strcpy(tar->name, component_name);
            tar->type = FILE_REGULAR;
            struct vnode* vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
            vnode->f_ops = dir_node->f_ops;
            vnode->v_ops = dir_node->v_ops;
            vnode->internal = (void*)tar;
            tmpfs_set_entry(tar, vnode, component_name);
            tar->buf = (tmpfs_buf*)kmalloc(sizeof(tmpfs_buf));
            tar->buf->size = 0;
            *target = tar->vnode;
            return 1;
        }
    }
    return -1; // fail
}
