#include "tmpfs.h"
#include "string.h"
#include "mm.h"
#include "io.h"

int tmpfs_write (struct file* file, const void* buf, u32 len) {
    void *base = file->vnode->internal + file->f_pos;
    strncopy(base, buf, len + 1);
    file->f_pos += len;
    return len;
}
int tmpfs_read (struct file* file, void* buf, u32 len) {
    if (len <= 1)
        return 0;

    void *base = file->vnode->internal + file->f_pos;
    u32 size = len < strlen(base) + 1 ? len : strlen(base) + 1;
    strncopy(buf, base, size);
    file->f_ops += size - 1;
    return size - 1;
}

int tmpfs_lookup (struct vnode* dir_node, struct vnode** target, const char* component_name) {
    if (!strlen(component_name))
        return 0;

    char buffer[256];
    long next_solidus = strfind(component_name, '/');
    if (next_solidus == -1)
        strncopy(buffer, component_name, strlen(component_name) + 1);
    else
        strncopy(buffer, component_name, next_solidus + 1);

    for (struct vnode_child *child = dir_node->childs; child; child = child->next) {
        if (!strcmp(buffer, child->child->name)) {
            *target = child->child;

            /* find next level child node */
            if (next_solidus != -1 && component_name[next_solidus + 1] != '\0') {
                struct vnode *tmp;
                int flag = (*target)->v_ops->lookup(*target, &tmp, &component_name[next_solidus + 1]);
                *target = tmp;
                return flag;
            }
            return 0;
        }
    }
    *target = NULL;
    return -1;
}

struct vnode *new_vnode (struct vnode *parent, char *name) {
    struct vnode *node = m_malloc(sizeof(struct vnode));
    *node = (struct vnode) {
        .mount = parent->mount,
        .v_ops = parent->v_ops,
        .f_ops = parent->f_ops,
        .childs = NULL,
        .name = strdup(name),
        .internal = NULL
    };
    return node;
}

int tmpfs_create (struct vnode* dir_node, struct vnode** target, const char* component_name) {
    long next_solidus = strfind(component_name, '/');
    /* create a new folder */
    if (next_solidus == -1) {
        *target = new_vnode(dir_node, (char *)component_name);
    }
    /* create folder recursively */
    else {
        struct vnode *tmp;
        char buffer[256];
        strncopy(buffer, component_name, next_solidus + 1);
        int flag = dir_node->v_ops->lookup(dir_node, &tmp, buffer);

        /* no such directory */
        // TODO: create whole path
        if (flag) {
            *target = NULL;
            return -1;
        }

        tmp->v_ops->create(tmp, target, &component_name[next_solidus + 1]);
        return 0;
    }

    if (*target) {
        struct vnode_child *list = m_malloc(sizeof(struct vnode_child));
        *list = (struct vnode_child) {
            .child = *target,
            .next = NULL,
        };

        if (dir_node->childs) {
            list->next = dir_node->childs;
            dir_node->childs = list;
        }
        else {
            dir_node->childs = list;
        }
    }
    return 0;
}
