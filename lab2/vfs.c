#include "include/vfs.h"
#include "include/tmpfs.h"
#include "include/csched.h"
#include "include/cutils.h"
#include "include/mm.h"

struct mount* rootfs_mount;
struct mount mnt_tab[MAX_MNT_NR];
struct filesystem fs_tab[MAX_FS_NR];
struct vnode vnode_pool[MAX_VNODE_NR];
struct file openfile_tab[MAX_FD_NR];
extern struct task *current;

void init_mnttab()
{
    for (int i = 0; i < MAX_MNT_NR; ++i)
        mnt_tab[i].root = 0;
}

struct mount *new_mount()
{
    for (int i = 0; i < MAX_MNT_NR; ++i)
        if (!(mnt_tab[i].root))
            return &mnt_tab[i];
    return 0; // NULL
}

void init_oftab()
{
    for (int i = 0; i < MAX_FD_NR; ++i) {
        openfile_tab[i].f_pos = 0;
        openfile_tab[i].flags = 0;
        openfile_tab[i].child_iter = 0;
        openfile_tab[i].ref = 0;
    }
}

struct file *new_file()
{
    for (int i = 0; i < MAX_FD_NR; ++i)
        if (!(openfile_tab[i].ref)) {
            openfile_tab[i].ref = 1;
            return &openfile_tab[i];
        }
    return 0; // NULL
}

void init_fstab()
{
    for (int i = 0; i < MAX_FS_NR; ++i)
        fs_tab[i].name = 0;
}

int register_filesystem(char *name, unsigned long setup_mount)
{ // register the file system to the kernel.
    struct filesystem *fs;
    for (int i = 0; i < MAX_FS_NR; ++i) {
        fs = &fs_tab[i];
        if (!(fs->name)) {
            fs->name = name;
            fs->cnt = 0;
            fs->setup_mount = (int (*)(struct filesystem*, struct mount*))setup_mount;
            return 0;
        }
    }
    return -1;
}

struct filesystem *get_filesystem(const char *filesystem)
{
    for (int i = 0; i < MAX_FS_NR; ++i)
        if (!(strcmp(filesystem, fs_tab[i].name)))
            return &fs_tab[i];
    return 0; // NULL
}

int vfs_mount(const char* device, const char* mountpoint,
    const char* filesystem)
{ // TODO: device for device-based fs
    struct vnode *mnt_node, *new_fs_root;
    struct filesystem *fs;
    struct mount *new_mnt;

    new_mnt = new_mount();
    mnt_node = get_vnode(mountpoint, 1);
    new_fs_root = new_vnode();
    new_fs_root->on = mnt_node;
    mnt_node->by = new_fs_root;
    fs = get_filesystem(filesystem);
    fs->setup_mount(fs, new_mnt, new_fs_root);

    return 0;
}

int vfs_umount(const char* mountpoint)
{
    struct vnode *mnt_node;

    mnt_node = get_vnode(mountpoint, 1);
    if (!(mnt_node->by))
        return 1; // mountpoint wasn't mounted by anyone.
    // mnt_node->by->v_ops->setup_umount(mnt_node->by->mount);
    mnt_node->by->on = 0;
    mnt_node->by = 0;
    return 0;
}

void init_root_filesystem() {
    // init vnode
    // mount the vnode 
    struct filesystem *root_fs = get_filesystem("tmpfs");
    rootfs_mount = new_mount();
    struct vnode *root = new_vnode();
    root_fs->setup_mount(root_fs, rootfs_mount, root);
}

void init_vnode_pool()
{
    for (int i = 0; i < MAX_VNODE_NR; ++i)
        vnode_pool[i].mount = 0;
}

struct vnode *new_vnode()
{
    struct vnode *vnode;
    for (int i = 0; i < MAX_VNODE_NR; ++i) {
        vnode = &vnode_pool[i];
        if (!(vnode->mount)) {
            vnode->internal = 0;
            vnode->on = 0;
            vnode->by = 0;
            vnode->f_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));
            vnode->v_ops = (struct vnode_operations*)kmalloc(sizeof(struct vnode_operations));
            return vnode;
        }
    }
    return 0; // NULL
}

char *get_next_component(char *str, char delimeter)
{
    char *ch = str;
    while (*ch != delimeter)
        ch++;
    return ch + 1;
}

struct vnode *get_vnode(const char *pathname, int level)
{ 
    // Example: get_vnode("/bin/aloha/file.c", root, type)
    // level == 3:  return vnode of "/bin"
    // level == 2:  return vnode of "/bin/aloha"
    // level == 1: return vnode of "/bin/aloha/file.c"
    char buf[32], *buf_ptr;
    struct vnode *vnode_dir, *vnode_walk;
    int folder_nr;

    strcpy(buf, pathname);
    buf_ptr = buf;

    folder_nr = get_level(pathname) - level;
    // if ((folder_nr = get_level(pathname) - level) < 0)
    //     return 0; // NULL

    /* Decide starting directory to lookup. */ 
    if (buf[0] == '/') { // "/x/y"
        vnode_dir = rootfs_mount->root;
        buf_ptr = get_next_component(buf_ptr, '/');
    }
    else if (buf[0] == '.' && buf[1] == '.') { // "../x/y"
        current->wd->v_ops->get_parent(current->wd, &vnode_dir);
        buf_ptr = get_next_component(buf_ptr, '/');
    }
    else { // "./x/y" or "x/y"
        vnode_dir = current->wd;
        if (buf[0] == '.' && buf[1] == '/')
            buf_ptr = get_next_component(buf_ptr, '/');
    }

    for (char *ch = buf; *ch != '\0'; ch++)
        if (*ch == '/')
            *ch = '\0';

    /* Lookup downward */
    while ((folder_nr--) > 0) {
        vnode_dir->v_ops->lookup(vnode_dir, &vnode_walk, buf_ptr);
        if (!vnode_walk)
            return 0; // NULL
        buf_ptr = get_next_component(buf_ptr, '\0');
        vnode_dir = vnode_walk;
        vnode_walk = 0;
    }
    return vnode_dir;
}

char *get_file_component(const char *pathname)
{ // Example: get_file("/bin/aloha/file.c") returns "file.c"
    int level = get_level(pathname) - 1;
    char *ch;
    for (ch = pathname; *ch != '\0'; ch++) {
        if (*ch == '/') {
            if (!(--level))
                break;
        }
    }
    return ch + 1;
}

struct file* vfs_open(const char* pathname, int flags)
{   // Assume user will only create file with existed folder.
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    struct vnode *start_vnode, *target;
    struct file *file;

    /* Decide target: 1. new created one 2. existed one */
    if (flags & O_CREAT) { // Don't worry, create will change target value.
        start_vnode = get_vnode(pathname, 2);
        start_vnode->v_ops->create(start_vnode, &target,
            get_file_component(pathname), REG_FILE);
    } else {
        if (!(target = get_vnode(pathname, 1)))
            return 0;
    }
    file = new_file();
    file->vnode = target;
    file->f_ops = target->f_ops;
    file->vnode->v_ops->get_child(file->vnode, &(file->child_iter));

    return file;
}

int vfs_close(struct file* file) {
    // 1. release the file descriptor
    file->ref--;
    if (file->ref <= 0) {
        file->ref = 0;
        file->f_pos = 0;
        file->flags = 0;
        return 0;
    }
    return 1; // Still has someone used it.
}

int vfs_write(struct file* file, const void* buf, int len) {
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file* file, void* buf, int len) {
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
    return file->f_ops->read(file, buf, len);
}

int vfs_mkdir(const char *pathname, int mode)
{
    struct vnode *start_vnode, *target;

    if (!(start_vnode = get_vnode(pathname, 2))){
        uart_send_string("Error from vfs_mkdir: get_vnode failed.\r\n");
        return 1;
    }
    start_vnode->v_ops->create(start_vnode, &target,
        get_file_component(pathname), REG_DIR);

    return 0;
}

int vfs_chdir(const char *pathname)
{
    current->wd = get_vnode(pathname, 1);
    return 0;
}
