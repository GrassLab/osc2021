#include "include/tmpfs.h"
#include "include/vfs.h"
#include "include/cutils.h"
#include "include/dtp.h"
#include "include/mm.h"

struct tmpent tmpent_pool[MAX_TMPENT_NR];
struct tmpent tmp_root;


void init_tmpent_pool()
{
    for (int i = 0; i < MAX_TMPENT_NR; ++i)
        tmpent_pool[i].free = 1;
}

struct tmpent *new_tmpent()
{
    struct tmpent *tmpent;
    for (int i = 0; i < MAX_TMPENT_NR; ++i) {
        tmpent = &tmpent_pool[i];
        if (tmpent->free) {
            tmpent->free = 0;
            tmpent->rsib = 0;
            tmpent->parent = 0;
            tmpent->child = 0;
            return tmpent;
        }
    }
    return 0; // NULL
}


int get_level(const char *pathname)
{
    int cnt = 1;
    const char *ch;
    for (ch = pathname; *ch != '\0'; ch++)
        if (*ch == '/')
            cnt++;
    return cnt;
}

char *get_level_name(const char *pathname, int level)
{
    int i = level;
    const char *ch = pathname;
    while (i > 1) {
        if (*ch == '/')
            i--;
        ch++;
    }
    return ch;
}


int tmpfs_write(struct file* file, const void* buf, int len)
{ // Assume file size wouldn't grow over 4096 bytes.
    struct tmpent *tar;
    unsigned long top;
    int inc;

    tar = (struct tmpent*)(file->vnode->internal);
    if (tar->type == DIRENT)
        return 0;
    top = (unsigned long)(tar->data_start + tar->size);
    inc = (int)((unsigned long)(tar->data_start + file->f_pos + len) - top);
    if (inc)
        tar->size += inc;
    memcpy(tar->data_start + file->f_pos, (char*)buf, len);
    file->f_pos += len;

    return len;
}

int tmpfs_read(struct file* file, void* buf, int len)
{
    struct tmpent *tar;
    int remain, len_r;

    tar = (struct tmpent*)(file->vnode->internal);
    if (tar->type == DIRENT)
        return 0;
    remain = (tar->data_start + tar->size) -
            (tar->data_start + file->f_pos);
    len_r = remain >= len ? len : remain;
    memcpy((char*)buf, tar->data_start + file->f_pos, len_r);
    file->f_pos += len_r;
    
    return len_r;
}

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target,
    const char* component_name)
{
    struct tmpent *ent, *walk;

    if (dir_node->by) // dir_node is mounted by someone
        return dir_node->by->v_ops->lookup(dir_node->by, target, component_name);
    ent = (struct tmpent*)(dir_node->internal);
    walk = ent->child;
    while (walk) {
        if (!strcmp(walk->name, component_name)) {
            *target = walk->vnode;
            return 0;
        }
        walk = walk->rsib;
    }
    return 1;
}

int tmpfs_create(struct vnode* dir_node, struct vnode** target,
    const char* component_name, int type)
{
    struct tmpent *tmpdir, *new;

    tmpdir = (struct tmpent*)(dir_node->internal);
    new = new_tmpent();
    // Setting name
    strcpy(new->name, component_name);
    // Add into directory
    new->rsib = tmpdir->child;
    new->parent = tmpdir;
    tmpdir->child = new;
    // Setting vnode
    new->vnode = new_vnode();
    *target = new->vnode;
    (*target)->mount = dir_node->mount;
    (*target)->v_ops = dir_node->v_ops;
    (*target)->f_ops = dir_node->f_ops;
    (*target)->internal = new;
    // Setting type
    new->type = type;
    if (type == FILEENT) {
        new->data_start = kmalloc(0x1000);
        new->size = 0;
    }
    return 0;
}

int tmpfs_get_parent(struct vnode* dir_node, struct vnode** target)
{
    struct tmpent *tmpdir;

    tmpdir = (struct tmpent*)(dir_node->internal);
    if (tmpdir == tmpdir->parent) { // means tmpdir is a root
        if (!(dir_node->on)) {
            *target = dir_node;
            return 0;
        }
        // dir_node is on someone.
        return dir_node->on->v_ops->get_parent(dir_node->on, target);
    }
    *target = tmpdir->parent->vnode;
    return 0;
}

int tmpfs_stat(struct vnode *vnode, struct dentry *dent)
{
    struct tmpent *self;

    self = (struct tmpent*)(vnode->internal);
    memcpy(dent->name, self->name, 16);
    dent->size = self->size;
    dent->type = self->type;
    return 0;
}

int tmpfs_get_rsib(struct vnode *vnode, struct vnode **target)
{
    struct tmpent *self, *rsib;

    self = (struct tmpent*)(vnode->internal);
    if (!(rsib = self->rsib))
        *target = 0;
    else
        *target = rsib->vnode; // may be NULL
    return 0;
}

int tmpfs_get_child(struct vnode *vnode, struct vnode **target)
{
    struct tmpent *self;

    if (vnode->by)
        return vnode->by->v_ops->get_child(vnode->by, target);
    self = (struct tmpent*)(vnode->internal);
    if (!(self->child))
        *target = 0;
    else
        *target = self->child->vnode; // may be NULL
    return 0;
}

int tmpfs_setup_mount(struct filesystem* fs,
    struct mount* mount, struct vnode *root)
{
    mount->root = root;
    mount->fs = fs;
    mount->root->mount = mount;
    mount->root->type = REG_DIR;
    mount->root->internal = (void*)&tmp_root;
    if (!(mount->fs->cnt++)) // only firt time setup will set root.vnode
        tmp_root.vnode = mount->root;
    mount->root->f_ops->read = tmpfs_read;
    mount->root->f_ops->write = tmpfs_write;
    mount->root->v_ops->create = tmpfs_create;
    mount->root->v_ops->lookup = tmpfs_lookup;
    mount->root->v_ops->get_parent = tmpfs_get_parent;
    mount->root->v_ops->stat = tmpfs_stat;
    mount->root->v_ops->get_rsib = tmpfs_get_rsib;
    mount->root->v_ops->get_child = tmpfs_get_child;
    return 0;
}

int init_tmpfs()
{
    struct cpio_newc_header *now; //, *prev;
    int filesize, namesize, mode;
    char *name_start, *data_start;
    struct tmpent *tmpent_now, *tmpent_prev;
    int level_prev, level_now, level_diff;

    init_tmpent_pool();

    tmp_root.parent = &tmp_root;
    tmp_root.rsib = &tmp_root;
    tmp_root.type = DIRENT;

    /* Iterate initramfs */
    now = (struct cpio_newc_header*)INITRAMFS_BASE;
    level_prev = 0;
    tmpent_prev = &tmp_root;
    while (1) {
        namesize = hex_string_to_int(now->c_namesize, 8);
        filesize = hex_string_to_int(now->c_filesize, 8);
        name_start = ((char *)now) + sizeof(struct cpio_newc_header);
        data_start = align_upper(name_start + namesize, 4);
        if (!strcmp(name_start, "TRAILER!!!"))
            break;
        level_now = get_level(name_start);
        tmpent_now = new_tmpent();

        /* Setting tmpent.name */
        strcpy(tmpent_now->name, get_level_name(name_start, level_now));

        /* Setting tmpent.link */
        if (level_prev < level_now) { // prev is parent
            tmpent_prev->child = tmpent_now;
            tmpent_now->parent = tmpent_prev;
        } else if (level_prev == level_now) { // prev is sibling
            tmpent_prev->rsib = tmpent_now;
            tmpent_now->parent = tmpent_prev->parent;
        } else {
            level_diff = level_prev - level_now;
            while (level_diff--)
                tmpent_prev = tmpent_prev->parent;
            tmpent_prev->rsib = tmpent_now;
            tmpent_now->parent = tmpent_prev->parent;
        }

        /* Setting tmpent.type */
        mode = hex_string_to_int(now->c_mode, 8);
        if (mode == 16888) { // directory
            tmpent_now->type = DIRENT;
        } else if (mode == 33272) { // regular file
            tmpent_now->type = FILEENT;
            tmpent_now->data_start = kmalloc(0x1000);
            tmpent_now->size = filesize;
            memcpy(tmpent_now->data_start, data_start, filesize);
        }
        /* Setting tmpent.vnode */
        tmpent_now->vnode = new_vnode();
        tmpent_now->vnode->mount = tmpent_prev->vnode->mount;
        tmpent_now->vnode->v_ops = tmpent_prev->vnode->v_ops;
        tmpent_now->vnode->f_ops = tmpent_prev->vnode->f_ops;
        tmpent_now->vnode->internal = tmpent_now;

        level_prev = level_now;
        tmpent_prev = tmpent_now;
        // prev = now;
        now = (struct cpio_newc_header*)align_upper(data_start + filesize, 4);
    }

    return 0;
}

