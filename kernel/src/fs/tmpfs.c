#include "vfs.h"
#include "tmpfs.h"
#include "dynamic_alloc.h"
#include "string.h"
#include "cpio.h"
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
    cpio_init_fs(mount->root);

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
    if (internal->name[0] == '/' && internal->l_child) {
        if (component_name[0] == '/') {
            component_name++; // shift out first character
        }
        
        return tmpfs_lookup(internal->l_child, target, component_name);
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
        if (last_slash == 0) {
            strncpy(path, "/", 1);
        } else {
            strncpy(path, component_name, last_slash);
        }

        if (tmpfs_lookup(dir_node, &parent, path) != 0) {
            printf("failed here\n");
            return -1; 
        }

        // shift to the last fragment
        component_name += (last_slash + 1);
    }

    // component_name is now the last fragment
    struct vnode *vnode = create_tmpfs_vnode(parent, component_name);
    append_child(parent, vnode);

    if (target) {
        *target = vnode;
    }

    return 0;
}


int tmpfs_write (struct file* file, const void* buf, size_t len)
{
    struct tmpfs_internal *internal = file->vnode->internal;
    char *start = internal->start_addr;
    start += file->f_pos;

    size_t remain_len = internal->size - file->f_pos;
    internal->size += (len > remain_len ? (len - remain_len) : 0);

    strncpy(start, buf, len);

    file->f_pos += len;

    return len;
}

int tmpfs_read (struct file* file, void* buf, size_t len)
{
    struct tmpfs_internal *internal = file->vnode->internal;
    char *start = internal->start_addr;

    start += file->f_pos;

    size_t remain_len = internal->size - file->f_pos;
    size_t cpy_len = remain_len >= len ? len : remain_len; // bound len

    strncpy(buf, start, cpy_len);

    file->f_pos += cpy_len;

    return cpy_len;
}

struct tmpfs_internal *create_tmpfs_vnode_internal(const char *name)
{
    struct tmpfs_internal *internal = malloc(sizeof(struct tmpfs_internal));
    strcpy(internal->name, name);

    internal->size = 0;
    internal->start_addr = malloc(256);

    return internal;
}

struct vnode *create_tmpfs_vnode(struct vnode *parent, const char *name)
{
    struct vnode *vnode = malloc(sizeof(struct vnode));
    vnode->f_ops = parent->f_ops;
    vnode->mount = parent->mount;
    vnode->v_ops = parent->v_ops;

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
    // printf("1\n");
    struct tmpfs_internal *parent_internal = parent->internal;
    if (!parent_internal->l_child) {
        parent_internal->l_child = node;
    } else {
        struct vnode *tmp_node = parent_internal->l_child;
        struct tmpfs_internal *tmp_internal = tmp_node->internal; // this line has broken on rpi
        while (tmp_internal->r_sibling) {
            tmp_node = tmp_internal->r_sibling;
            tmp_internal = tmp_node->internal;
        }

        // tmp_internal = tmp_node->internal;
        tmp_internal->r_sibling = node;


        // print child
        // tmp_node = parent_internal->l_child;
        // tmp_internal = tmp_node->internal;

        // while (tmp_node) {
        //     printf("%s -> ", tmp_internal->name);

        //     tmp_node = tmp_internal->r_sibling;
        //     tmp_internal = tmp_node->internal;
        // }
        // printf("\n");
    }
}