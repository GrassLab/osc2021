#include "vfs.h"
#include "tmpfs.h"
#include "dynamic_alloc.h"
#include "string.h"

#include "io.h"

struct filesystem tmpfs = {
    .name = "tmpfs",
    .setup_mount = tmpfs_setup
};

struct vnode_operations tmpfs_v_ops = {
    .lookup = tmpfs_lookup,
    .create = tmpfs_create
};

struct file_operations tmpfs_f_ops = {
    .write = tmpfs_write,
    .read = tmpfs_read
};

int tmpfs_setup(struct filesystem *fs, struct mount* mount)
{
    // migrate root from previous filesystem to tmpfs
    mount->fs = fs;
    mount->root->mount = mount;
    mount->root->v_ops = &tmpfs_v_ops;
    mount->root->f_ops = &tmpfs_f_ops;

    // create root internal
    mount->root->internal = create_tmpfs_vnode_internal("/");
    
    return 0;
}

int tmpfs_lookup(struct vnode* dir_node, struct vnode **target, const char* component_name)
{
    struct tmpfs_internal *internal = dir_node->internal;

    // terminate condition
    if (strcmp(internal->name, component_name) == 0) {
        *target = dir_node;
        return 0;
    }

    // if it's root, find child
    if (component_name[0] == '/') {
        if (internal->name[0] == '/' && internal->l_child) {
            component_name++; // shift out first character
            return tmpfs_lookup(internal->l_child, target, component_name);
        } else {
            return -1; // not found
        }
    }

    // get first fragment
    char frag[100] = { 0 };
    int offset = get_first_frag(frag, component_name);
    component_name += offset;

    struct vnode *tmp_vnode = dir_node;
    
    // search each siblings
    while (((struct tmpfs_internal *)tmp_vnode->internal)->r_sibling) {
        struct vnode *r_sibling = ((struct tmpfs_internal *)tmp_vnode->internal)->r_sibling;
        struct tmpfs_internal *r_sibling_internal = r_sibling->internal;

        if (strcmp(r_sibling_internal->name, frag) == 0) {
            // sibling match
            if (strlen(component_name) == 0) {
                *target = r_sibling;
                return 0;
            }

            // lookup its child
            return tmpfs_lookup(r_sibling_internal->l_child, target, component_name);
        }

        // not this sibling, try next
        tmp_vnode = r_sibling;
    }


    // finally not found
    return -1;
}


int tmpfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    // separate component name into path and filename
    int len = strlen(component_name), 
        last_slash = len, 
        i = 0;

    // get the last slash index    
    for (i = 0; i < len; i++) {
        if (component_name[i] == '/') {
            last_slash = i;
        }
    }

    struct vnode *parent;

    // no slash in component name
    if (last_slash == len) {
        parent = dir_node;
        // whole component name is filename
    } else {
        char path[100] = { 0 }; // used to get parent
        strncpy(path, component_name, last_slash);
        
        if (!tmpfs_lookup(dir_node, &parent, path)) {
            return -1;
        }

        // shift to the last fragment
        component_name += (last_slash + 1);
    }

    // component_name is now the last fragment
    struct vnode *vnode = create_tmpfs_vnode(component_name);
    append_child(parent, vnode);

    if (target) {
        *target = vnode;
    }

    return 0;
}


int tmpfs_write (struct file* file, const void* buf, size_t len)
{
    return 0;
}

int tmpfs_read (struct file* file, void* buf, size_t len)
{
    return 0;
}

struct tmpfs_internal *create_tmpfs_vnode_internal(const char *name)
{
    struct tmpfs_internal *internal = malloc(sizeof(struct tmpfs_internal));
    strcpy(internal->name, name);

    return internal;
}

struct vnode *create_tmpfs_vnode(const char *name)
{
    struct vnode *vnode = malloc(sizeof(struct vnode));
    vnode->internal = create_tmpfs_vnode_internal(name);

    return vnode;
}

// return offset
int get_first_frag(char *buffer, const char *component_name)
{
    int i = 0;

    while (component_name[i] != '/' && i < strlen(component_name)) {
        i++;
    }

    // '/' is at index i, or end of string '/0' is at index i
    strncpy(buffer, component_name, i);

    return (i + 1);
}

void append_child(struct vnode *parent, struct vnode *node)
{
    struct tmpfs_internal *parent_internal = parent->internal;
    if (!parent_internal->l_child) {
        parent_internal->l_child = node;
    } else {
        struct vnode *tmp_node = parent_internal->l_child;
        struct tmpfs_internal *tmp_internal = tmp_node->internal;

        while (tmp_internal->r_sibling) {
            tmp_node = tmp_internal->r_sibling;
            tmp_internal = tmp_node->internal;
        }

        tmp_internal = tmp_node->internal;
        tmp_internal->r_sibling = node;
    }
}