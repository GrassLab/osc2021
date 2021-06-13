#include "tmpfs.h"
#include "allocator.h"
#include "../lib/string.h"
#include "../lib/uart.h"

struct filesystem tmpfs = {
    .name = "tmpfs",
    .setup_mount = tmpfs_setup
};

struct file_operations tmp_f_ops = {
    .write = tmpfs_write,
    .read = tmpfs_read
};

struct vnode_operations tmp_v_ops = {
    .lookup = tmpfs_lookup,
    .create = tmpfs_create
};

int tmpfs_setup(struct filesystem * fs, struct mount * mount)
{
    mount->fs = fs;
    mount->root->mount = mount;
    mount->root->v_ops = &tmp_v_ops;
    mount->root->f_ops = &tmp_f_ops;
    mount->root->internel = NULL;

    return 0;
}

int tmpfs_lookup(struct vnode * dir_node, struct vnode ** target, const char * component_name)
{
    struct vnode * tmp_node = dir_node;

    int count = 0;
    while (tmp_node->internel != NULL && ((struct tmpfs_internel *)(tmp_node->internel))->next_sibling != NULL)
    {
        if (!strcmp(component_name, ((struct tmpfs_internel *)(tmp_node->internel))->name))
        {
            break;      
        }
        tmp_node = ((struct tmpfs_internel *)(tmp_node->internel))->next_sibling;
    }

    if (tmp_node->internel == NULL || strcmp(component_name, ((struct tmpfs_internel *)(tmp_node->internel))->name))
    {
        return -1;
    }
    else
    {
        *target = tmp_node;
        return 0;
    }
}

int tmpfs_create(struct vnode * dir_node, struct vnode ** target, const char * component_name)
{
    struct vnode * tmp_node = NULL;

    tmp_node = tmpfs_create_vnode(dir_node, component_name);
    append_child(dir_node, tmp_node);
    
    *target = tmp_node;

    return 0;
}

int tmpfs_read(struct file * file, void * buf, size_t len)
{
    struct tmpfs_internel * internel = file->vnode->internel;
    char * start_addr = internel->addr;
    char * buf_addr = (char*)buf;
    int max_len = len < internel->size - file->f_pos ? len : internel->size - file->f_pos;
 
    start_addr += file->f_pos;

    for (int i = 0; i < max_len; ++i)
    {
        buf_addr[i] = start_addr[i];
    }

    file->f_pos += max_len;

    return max_len;
}

int tmpfs_write(struct file * file, const void * buf, size_t len)
{
    struct tmpfs_internel * internel = file->vnode->internel;
    char * start_addr = internel->addr;
    char * buf_addr = (char*)buf;
    int max_len = file->f_pos + len > TMPFS_SIZE ? (file->f_pos + len) - TMPFS_SIZE : len;

    start_addr += file->f_pos;

    for (int i = 0; i < max_len; ++i)
    {
        start_addr[i] = buf_addr[i];
    }

    internel->size += max_len;
    file->f_pos += max_len;

    return max_len;
}

struct vnode * tmpfs_create_vnode(struct vnode * src, const char * name)
{
    struct vnode * node = (struct vnode *)buddy_alloc(sizeof(struct vnode));
    
    node->mount = src->mount;
    node->v_ops = src->v_ops;
    node->f_ops = src->f_ops;
    node->internel = (void *)tmpfs_create_internel(name);

    return node;
}

struct tmpfs_internel * tmpfs_create_internel(const char * name)
{
    struct tmpfs_internel * internel = (struct tmpfs_internel *)buddy_alloc((sizeof(struct tmpfs_internel)));

    strcpy(name, internel->name);
    internel->size = 0;
    internel->addr = buddy_alloc(TMPFS_SIZE);
    internel->next_sibling = NULL;

    return internel;
}

void append_child(struct vnode * parent, struct vnode * child)
{
    struct tmpfs_internel * internel = (struct tmpfs_internel *)(parent->internel);

    if (internel == NULL)
    {
        *parent = *child;
    }
    else
    {
        while(internel->next_sibling != NULL)
        {
            if (internel->next_sibling->internel != NULL)
            {
                internel = (struct tmpfs_internel *)(internel->next_sibling->internel);
            }
            else
            {
                break;
            }
        }
        internel->next_sibling = child;
    }
}