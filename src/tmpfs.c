#include "tmpfs.h"
#include "slab.h"
#include "printf.h"

int tmpfs_write(struct file* file, const void* buf, int len) {
    int f_pos = file->f_pos;
    int *file_size = (int *)(file->vnode->internal);
    memcpy((unsigned long)buf, (unsigned long)file->vnode->internal + sizeof(int) + f_pos, len);
    file->f_pos += len;
    if(*file_size < file->f_pos) {
        *file_size = file->f_pos;
    }
    return len;
}

int tmpfs_read(struct file* file, void* buf, int len) {
    int f_pos = file->f_pos;
    int *file_size = (int *)(file->vnode->internal);
    if((f_pos + len) >= *file_size) {
        len = *file_size - f_pos;
    }
    memcpy((unsigned long)file->vnode->internal + sizeof(int) + f_pos, (unsigned long)buf, len);
    file->f_pos += len;
    return len;
}

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name) {
    struct directory *read_dir  = (struct directory *)dir_node->internal;
    struct direntry *iter_entry = read_dir->head;
    if(iter_entry == 0) {
        return 0;
    }
    while(iter_entry != 0) {
        if((iter_entry->entry->type == REG_FILE) && strcmp_eq(((struct direntry*)iter_entry)->name, component_name)) {
            *target = iter_entry->entry;
            return 1;
        }
        iter_entry = iter_entry->next;
    }
    return 0;
}

int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name, int type) {
    struct directory *read_dir     = (struct directory *)dir_node->internal;
    struct direntry  *create_entry = obj_allocate(sizeof(struct direntry));
    
    *target = (struct vnode *)obj_allocate(sizeof(struct vnode));
    if(type == REG_FILE) {
        (*target)->type               = REG_FILE;
        (*target)->internal           = (struct block*)get_free_pages(1);
        *(int *)((*target)->internal) = 0; // set the file size
    }
    (*target)->mount = dir_node->mount;
    (*target)->f_ops = (struct file_operations *)obj_allocate(sizeof(struct file_operations));
    (*target)->f_ops->read   = tmpfs_read;
    (*target)->f_ops->write  = tmpfs_write;
    (*target)->v_ops = (struct vnode_operations *)obj_allocate(sizeof(struct vnode_operations));
    (*target)->v_ops->create = tmpfs_create;
    (*target)->v_ops->lookup = tmpfs_lookup;

    create_entry->name = (char *)obj_allocate(sizeof(char) * 20);
    strcpy(create_entry->name, component_name);
    create_entry->entry = (*target);
    if(read_dir->head == 0) {
        read_dir->head = create_entry;
        read_dir->head->next = 0;
        read_dir->tail = read_dir->head;
    }
    else {
        read_dir->tail->next = create_entry;
        read_dir->tail = read_dir->tail->next;
        read_dir->tail->next = 0;
    }
    return 1;
}

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount) {
    mount->root                = (struct vnode *)obj_allocate(sizeof(struct vnode));
    mount->fs                  = fs;
    mount->root->mount         = mount;
    mount->root->type          = REG_DIR;
    mount->root->internal      = (struct directory *)obj_allocate(sizeof(struct directory));
    mount->root->f_ops         = (struct file_operations *)obj_allocate(sizeof(struct file_operations));
    mount->root->f_ops->read   = tmpfs_read;
    mount->root->f_ops->write  = tmpfs_write;
    mount->root->v_ops->create = tmpfs_create;
    mount->root->v_ops->lookup = tmpfs_lookup;
    return 1;
}

void setup_tmpfs(struct filesystem **fs, char *name) {
    (*fs) = (struct filesystem *)obj_allocate(sizeof(struct filesystem));
    (*fs)->name = name;
    (*fs)->setup_mount = tmpfs_setup_mount;
}