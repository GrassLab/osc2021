#include "tmpfs.h"

struct filesystem tmpfs = {.name = "tmpfs", .setup_mount = setup_mount};
struct filesystem cpiofs = {.name = "cpiofs", .setup_mount = setup_mount};
static struct file_operations tmpfs_f_ops = {.write = write, .read = read, .filesize = filesize, .file_name = node_name, .content = content};
static struct vnode_operations tmpfs_v_ops = {.lookup = lookup, .create_file = create_file, .create_dir = create_dir, .list = list, .dir_name = node_name, .content = content, .set_parent = set_parent};

static int setup_mount(struct filesystem *fs, struct mount *mount){
    mount->root = malloc(sizeof(struct vnode));
    if(!mount->root){
        return -1;
    }
    mount->root->mount = mount;
    mount->root->v_ops = &tmpfs_v_ops;
    mount->root->f_ops = NULL;
    mount->root->internal = malloc(sizeof(tmpfs_node));
    if(!mount->root->internal){
        free(mount->root);
        return -1;
    }

    tmpfs_node *node = mount->root->internal;
    node->next = NULL;
    node->mode = TFS_DIR;
    node->name = malloc(2);
    if(!node->name){
        free(mount->root->internal);
        free(mount->root);
        return -1;
    }
    node->name[0] = '/';
    node->name[1] = '\0';
    node->content = NULL;
    node->content_size = 1;

    if(init_dir(mount->root, mount->root) < 0){
        free(node->name);
        free(mount->root->internal);
        free(mount->root);
        return -1;
    }

    mount->fs = fs;
    return 0;
}

static int lookup(struct vnode *dir_node, struct vnode **target, const char *component_name){
    struct vnode *vnode = (struct vnode *)((tmpfs_node *)dir_node->internal)->content;
    while(vnode){
        tmpfs_node *node = (tmpfs_node *)vnode->internal;
        if(!strcmp(component_name, node->name)){
            *target = vnode;
            if(node->mode & TFS_DFILE){
                return 1;
            }
            return 0;
        }
        vnode = node->next;
    }
    return -1;
}

static int create(struct vnode* dir_node, struct vnode **target, const char *component_name, uint64_t mode){
    struct vnode *dum_target;
    if(dir_node->v_ops->lookup(dir_node, &dum_target, component_name) == 0){
        printf("create: file/dir name '%s' exist!!!" NEW_LINE, component_name);
        return -2;
    }

    struct vnode *new = malloc(sizeof(struct vnode));
    if(!new){
        return -1;
    }
    new->mount = NULL;
    new->v_ops = NULL;
    new->f_ops = NULL;
    if(mode & TFS_DIR){
        new->v_ops = &tmpfs_v_ops;
    }else if(mode & TFS_FILE){
        new->f_ops = &tmpfs_f_ops;
    }
    new->internal = malloc(sizeof(tmpfs_node));
    if(!new->internal){
        free(new);
        return -1;
    }

    tmpfs_node *node = new->internal;
    node->next = NULL;
    node->mode = mode;
    node->name = malloc(strlen(component_name) + 1);
    if(!node->name){
        free(new->internal);
        free(new);
        return -1;
    }
    strncpy(node->name, component_name, strlen(component_name) + 1);
    node->content = NULL;
    node->content_size = 1;

    if(!((tmpfs_node *)dir_node->internal)->content){
        ((tmpfs_node *)dir_node->internal)->content = (char *)new;
    }else{
        tmpfs_node *cur = ((struct vnode *)((tmpfs_node *)dir_node->internal)->content)->internal;
        while(cur->next){
            cur = cur->next->internal;
        }
        cur->next = new;
    }
    *target = new;
    return 0;
}

static int create_file(struct vnode* dir_node, struct vnode **target, const char *component_name){
    //printf("create file '%s' in dir '%s'" NEW_LINE, component_name, ((tmpfs_node*)dir_node->internal)->name);
    return create(dir_node, target, component_name, TFS_FILE);
}

static int create_dir(struct vnode* dir_node, const char *component_name){
    //printf("create dir '%s' in dir '%s'" NEW_LINE, component_name, ((tmpfs_node*)dir_node->internal)->name);
    struct vnode *target;
    int ret = create(dir_node, &target, component_name, TFS_DIR);
    if(ret == 0){
        if(init_dir(target, dir_node) < 0){
            printf("nononononononononono" NEW_LINE);
            //rmdir
            //TODO
            //TODO
            //TODO
            //TODO
            //TODO
            //TODO
            //TODO
            //TODO
            //TODO
            //TODO
            //TODO
            //TODO
            //TODO
            //TODO
            return -1;
        }
    }
    return ret;
}

static int write(struct file *file, const void *buf, size_t len){
    tmpfs_node *node = (tmpfs_node *)file->vnode->internal;
    if(file->f_pos + len >= node->content_size){
        char *new = malloc(file->f_pos + len + 1);
        if(!new){
            return -1;
        }
        strncpy(new, node->content, file->f_pos);
        free(node->content);
        node->content = new;
        node->content_size = file->f_pos + len + 1;
    }
    strncpy(node->content + file->f_pos, buf, len);
    file->f_pos += len;
    return len;
}

static int read(struct file *file, void *buf, size_t len){
    tmpfs_node *node = (tmpfs_node *)file->vnode->internal;
    if(file->f_pos == (node->content_size - 1)){
        return -22;
    }
    if(file->f_pos + len >= node->content_size){
        len = node->content_size - 1 - file->f_pos;
    }
    strncpy(buf, node->content + file->f_pos, len);
    file->f_pos += len;
    return len;
}

static size_t filesize(struct file *file){
    return ((tmpfs_node *)file->vnode->internal)->content_size - 1;
}

static void list(struct vnode *dir_node){
    //printf("listing dir '%s' of %p" NEW_LINE, ((tmpfs_node*)dir_node->internal)->name, dir_node);
    struct vnode *vnode = (struct vnode *)((tmpfs_node *)dir_node->internal)->content;
    while(vnode){
        tmpfs_node *node = (tmpfs_node *)vnode->internal;
        if(node->mode & TFS_FILE){
            printf("  f  %lu  %s" NEW_LINE, node->content_size - 1, node->name);
        }else if(node->mode & TFS_DIR){
            printf("  d  %lu  %s" NEW_LINE, node->content_size - 1, node->name);
        }else{
            printf("  -  %lu  %s" NEW_LINE, node->content_size - 1, node->name);
        }
        vnode = node->next;
    }
}

static void node_name(struct vnode *vnode, char *buf){
    tmpfs_node *node = (tmpfs_node *)vnode->internal;
    strncpy(buf, node->name, -1);
}

static void *content(struct vnode *vnode){
    tmpfs_node *node = (tmpfs_node *)vnode->internal;
    return node->content;
}

static int init_dir(struct vnode *child, struct vnode *parent){
    struct vnode *dot = malloc(sizeof(struct vnode));
    tmpfs_node *dot_i = malloc(sizeof(tmpfs_node));
    struct vnode *dotdot = malloc(sizeof(struct vnode));
    tmpfs_node *dotdot_i = malloc(sizeof(tmpfs_node));
    if(!dot || !dot_i || !dotdot || !dotdot_i){
        free(dotdot_i);
        free(dotdot);
        free(dot_i);
        free(dot);
        return -1;
    }

    dot_i->name = malloc(2);
    dotdot_i->name = malloc(3);
    if(!dot_i->name || !dotdot_i->name){
        free(dotdot_i->name);
        free(dot_i->name);
        free(dotdot_i);
        free(dotdot);
        free(dot_i);
        free(dot);
        return -1;
    }

    dot->mount = NULL;
    dot->v_ops = child->v_ops;
    dot->f_ops = NULL;
    dot->internal = dot_i;

    dot_i->next = dotdot;
    dot_i->mode = TFS_DFILE;
    strncpy(dot_i->name, ".", -1);
    dot_i->content = (char *)dot;
    dot_i->content_size = 1;

    dotdot->mount = NULL;
    dotdot->v_ops = parent->v_ops;
    dotdot->f_ops = NULL;
    dotdot->internal = dotdot_i;

    dotdot_i->next = NULL;
    dotdot_i->mode = TFS_DFILE;
    strncpy(dotdot_i->name, "..", -1);
    dotdot_i->content = (char *)parent;
    dotdot_i->content_size = 1;

    ((tmpfs_node *)child->internal)->content = (char *)dot;
    return 0;
}

static void set_parent(struct vnode *mount, struct vnode *mountpoint){
    struct vnode *parent;
    mountpoint->v_ops->lookup(mountpoint, &parent, "..");
    void *parent_content = parent->v_ops->content(parent);
    struct vnode *dotdot;
    mount->v_ops->lookup(mount, &dotdot, "..");
    dotdot->v_ops = mountpoint->v_ops;
    ((tmpfs_node *)dotdot->internal)->content = parent_content;
}
