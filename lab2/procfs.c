#include "include/vfs.h"
#include "include/procfs.h"
#include "include/tmpfs.h"
#include "include/cutils.h"
#include "include/mm.h"
#include "include/csched.h"

struct procent procent_pool[MAX_PROCENT_NR];
struct procent proc_root;
extern struct task task_pool[];


void init_procent_pool()
{
    for (int i = 0; i < MAX_PROCENT_NR; ++i)
        procent_pool[i].free = 1;
}

struct procent *new_procent()
{
    struct procent *procent;
    for (int i = 0; i < MAX_PROCENT_NR; ++i) {
        procent = &procent_pool[i];
        if (procent->free) {
            procent->free = 0;
            procent->rsib = 0;
            procent->parent = 0;
            procent->child = 0;
            return procent;
        }
    }
    return 0; // NULL
}

int procfs_switch_write(struct file* file, const void* buf, int len)
{ // Assume file size wouldn't grow over 4096 bytes.
    struct procent *tar;

    tar = (struct procent*)(file->vnode->internal);
    if (tar->type == DIRENT)
        return 0;

    for (struct procent *walk = tar->parent->child; walk != 0; walk = walk->rsib) {
        if (!strcmp(walk->name, "hello")) {
            if (*((char*)buf) == '0')
                strcpy(walk->data_start, "hello");
            else if (*((char*)buf) == '1')
                strcpy(walk->data_start, "HELLO");
            // uart_send_string(walk->data_start);
            return 0;
        }
    }
    // memcpy(tar->data_start, (char*)buf, 1);
    return 1;
}

int procfs_write(struct file* file, const void* buf, int len)
{ // Assume file size wouldn't grow over 4096 bytes.
    struct procent *tar;
    unsigned long top;
    int inc;

    tar = (struct procent*)(file->vnode->internal);
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

int procfs_read(struct file* file, void* buf, int len)
{
    struct procent *tar;
    int remain, len_r;

    tar = (struct procent*)(file->vnode->internal);

    if (tar->type == DIRENT)
        return 0;
    remain = (tar->data_start + tar->size) -
            (tar->data_start + file->f_pos);
    len_r = remain >= len ? len : remain;
    memcpy((char*)buf, tar->data_start + file->f_pos, len_r);
    file->f_pos += len_r;
    
    return len_r;
}

int procfs_task_status_read(struct file* file, void* buf, int len)
{
    struct procent *tar;
    int remain, len_r;
    int pid;
    struct task *ts;

    tar = (struct procent*)(file->vnode->internal);
    pid = dec_string_to_int(tar->parent->name, strlen(tar->parent->name));

    ts = &task_pool[pid];
    if (ts->status == TASK_READY)
        strcpy(tar->data_start, "READY");
    else if (ts->status == TASK_RUNNING)
        strcpy(tar->data_start, "RUNNING");
    else if (ts->status == TASK_BLOCK)
        strcpy(tar->data_start, "BLOCK");
    else
        strcpy(tar->data_start, "UNKNOWN");
    tar->size = 8;
    remain = (tar->data_start + tar->size) -
            (tar->data_start + file->f_pos);
    len_r = remain >= len ? len : remain;
    memcpy((char*)buf, tar->data_start + file->f_pos, len_r);
    file->f_pos += len_r;
    
    return len_r;
}

// int procfs_lookup(struct vnode* dir_node, struct vnode** target,
//     const char* component_name)
// {
//     struct procent *ent, *walk;

//     if (dir_node->by) // dir_node is mounted by someone
//         return dir_node->by->v_ops->lookup(dir_node->by, target, component_name);
   
//     ent = (struct procent*)(dir_node->internal);
//     walk = ent->child;
//     while (walk) {
//         if (!strcmp(walk->name, component_name)) {
//             *target = walk->vnode;
//             return 0;
//         }
//         walk = walk->rsib;
//     }
//     /* lookup pid status */
//     if (!str_is_num(component_name)) {
//         *target = 0;
//         return 1;
//     }
//     struct vnode *task_vnode;
//     int pid = dec_string_to_int(component_name, strlen(component_name));
//     if (pid < MAX_TASK_NR && pid >= 0) {
//         if (!(task_pool[pid].free)) {
//             procfs_create(dir_node, target, component_name, DIRENT);
//             procfs_create(*target, &task_vnode, "status", FILEENT);
//             task_vnode->f_ops->read = procfs_task_status_read;
//             return 0;
//         }
//     }
//     *target = 0;
//     return 1;
// }

int procfs_lookup(struct vnode* dir_node, struct vnode** target,
    const char* component_name)
{
    struct procent *ent, *walk;

    if (dir_node->by) // dir_node is mounted by someone
        return dir_node->by->v_ops->lookup(dir_node->by, target, component_name);
   
    /* lookup pid status */
    if (!str_is_num(component_name)) {
        ent = (struct procent*)(dir_node->internal);
        walk = ent->child;
        while (walk) {
            if (!strcmp(walk->name, component_name)) {
                *target = walk->vnode;
                return 0;
            }
            walk = walk->rsib;
        }
    }
    else {
        struct vnode *task_vnode;
        int pid = dec_string_to_int(component_name, strlen(component_name));
        if (pid < MAX_TASK_NR && pid >= 0) {
            if (!(task_pool[pid].free)) {
                ent = (struct procent*)(dir_node->internal);
                walk = ent->child;
                while (walk) {
                    if (!strcmp(walk->name, component_name)) {
                        *target = walk->vnode;
                        return 0;
                    }
                    walk = walk->rsib;
                }
                // If the pid is searched first time, then it will come here.
                procfs_create(dir_node, target, component_name, DIRENT);
                procfs_create(*target, &task_vnode, "status", FILEENT);
                task_vnode->f_ops->read = procfs_task_status_read;
                return 0;
            }
        }
    }
    *target = 0;
    return 1;
}

int procfs_create(struct vnode* dir_node, struct vnode** target,
    const char* component_name, int type)
{
    struct procent *procdir, *new;

    procdir = (struct procent*)(dir_node->internal);
    new = new_procent();
    // Setting name
    strcpy(new->name, component_name);
    // Add into directory
    new->rsib = procdir->child;
    new->parent = procdir;
    procdir->child = new;
    // Setting vnode
    new->vnode = new_vnode();
    *target = new->vnode;
    (*target)->mount = dir_node->mount;
    (*target)->v_ops = dir_node->v_ops;
    // (*target)->f_ops = dir_node->f_ops;
    (*target)->f_ops->write = dir_node->f_ops->write;
    (*target)->f_ops->read = dir_node->f_ops->read;
    (*target)->internal = new;
    // Setting type
    new->type = type;
    new->size = 0;

    return 0;
}

int procfs_get_parent(struct vnode* dir_node, struct vnode** target)
{
    struct procent *procdir;

    procdir = (struct procent*)(dir_node->internal);
    if (procdir == procdir->parent) { // means tmpdir is a root
        if (!(dir_node->on)) {
            *target = dir_node;
            return 0;
        }
        // dir_node is on someone.
        return dir_node->on->v_ops->get_parent(dir_node->on, target);
    }
    *target = procdir->parent->vnode;
    return 0;
}

int procfs_stat(struct vnode *vnode, struct dentry *dent)
{
    struct procent *self;

    self = (struct procent*)(vnode->internal);
    memcpy(dent->name, self->name, 16);
    dent->size = self->size;
    dent->type = self->type;
    return 0;
}

int procfs_get_rsib(struct vnode *vnode, struct vnode **target)
{
    struct procent *self, *rsib;

    self = (struct procent*)(vnode->internal);
    if (!(rsib = self->rsib))
        *target = 0;
    else
        *target = rsib->vnode; // may be NULL
    return 0;
}

int procfs_get_child(struct vnode *vnode, struct vnode **target)
{
    struct procent *self;

    if (vnode->by)
        return vnode->by->v_ops->get_child(vnode->by, target);
    self = (struct procent*)(vnode->internal);
    if (!(self->child))
        *target = 0;
    else
        *target = self->child->vnode; // may be NULL
    return 0;
}

int procfs_setup_mount(struct filesystem* fs,
    struct mount* mount, struct vnode *root)
{
    mount->root = root;
    mount->fs = fs;
    mount->root->mount = mount;
    mount->root->type = REG_DIR;
    mount->root->internal = (void*)&proc_root;
    if (!(mount->fs->cnt++)) // only firt time setup will set root.vnode
        proc_root.vnode = mount->root;
    mount->root->f_ops->read = procfs_read;
    mount->root->f_ops->write = procfs_write;
    mount->root->v_ops->create = procfs_create;
    mount->root->v_ops->lookup = procfs_lookup;
    mount->root->v_ops->get_parent = procfs_get_parent;
    mount->root->v_ops->stat = procfs_stat;
    mount->root->v_ops->get_rsib = procfs_get_rsib;
    mount->root->v_ops->get_child = procfs_get_child;

    init_procfs();
    return 0;
}

int init_procfs()
{
    struct procent *procent_switch, *procent_hello;

    init_procent_pool();

    proc_root.parent = &proc_root;
    proc_root.rsib = &proc_root;
    proc_root.type = DIRENT;

    procent_switch = new_procent();
    strcpy(procent_switch->name, "switch");
    procent_switch->parent = &proc_root;
    proc_root.child = procent_switch;
    procent_switch->type = FILEENT;
    procent_switch->data_start[0] = '0';
    procent_switch->size = 1;
    procent_switch->vnode = new_vnode();
    procent_switch->vnode->mount = proc_root.vnode->mount;
    procent_switch->vnode->v_ops = proc_root.vnode->v_ops;
    procent_switch->vnode->f_ops->write = procfs_switch_write;
    procent_switch->vnode->f_ops->read = proc_root.vnode->f_ops->read;
    procent_switch->vnode->internal = procent_switch;

    procent_hello = new_procent();
    strcpy(procent_hello->name, "hello");
    procent_hello->parent = &proc_root;
    procent_switch->rsib = procent_hello;
    procent_hello->type = FILEENT;
    strcpy(procent_hello->data_start, "hello");
    procent_hello->size = 5;
    procent_hello->vnode = new_vnode();
    procent_hello->vnode->mount = proc_root.vnode->mount;
    // TODO: hello inherent parent's f_ops and v_ops, so should release
    // it's own f_ops and v_ops.
    procent_hello->vnode->v_ops = proc_root.vnode->v_ops;
    procent_hello->vnode->f_ops = proc_root.vnode->f_ops;
    procent_hello->vnode->internal = procent_hello;

    return 0;
}

