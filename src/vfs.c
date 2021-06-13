#include "vfs.h"
#include "slab.h"
#include "mm.h"
#include "printf.h"
#include "sched.h"

struct mount *rootfs_mount;
static struct filesystem *filesystem_array[10];
static char *filesystem_name[10];

void init_fs(){
	char name[20] = "fat32_fs1";
	struct filesystem *fat32_1;
	setup_fat32(&fat32_1, name);
	register_filesystem(fat32_1, "fat32_fs");
	init_root_filesystem();
}

int strcmp_eq(char *str1, char *str2) {
    while (1) {
        if ((str1[0] == '\0' && str2[0] == '\0') || (str1[0] == '\0' && str2[0] == 32) || (str1[0] == 32 && str2[0] == '\0')) {
            return 1;
        }
        if (*str1 != *str2) {
            return 0;
        }
        str1++;
        str2++;
    }
}

int strcpy(char *dest, char *source) {
    int index = 0;
    while(*source != '\0' && *source != '/') {
        *dest = *source;
        dest++;
        source++;
        index++;
    }
    if (*source == '/') {
        *dest = '\0';
        return index+1;
    }
    *dest = '\0';
    return 0;
}

struct vnode* entry_file_find(struct directory *dir, const char *comp_name) {
    if(dir->head == 0) return 0;
    struct direntry *iter_entry = dir->head;
    if(iter_entry == 0) return 0;
    while(iter_entry != 0) {
        if((iter_entry->entry->type == REG_FILE) && strcmp_eq(((struct direntry*)iter_entry)->name, comp_name)) {
            return iter_entry->entry;
        }
        iter_entry = iter_entry->next;
    }
    return 0;
}

int setup_mount(struct filesystem* fs, struct mount* mount) {
    mount->fs = fs;
    return 1;
}

void create_file_vnode(struct vnode *dir_vnode, char *comp_name, struct file *create_file) {
    struct vnode *create_vnode;
    dir_vnode->v_ops->create(dir_vnode, &create_vnode, comp_name, REG_FILE);
    create_file->f_pos     = 0;
    create_file->vnode = create_vnode;
    return;
}

int iterate_dir(struct directory *dir) {
    if(dir->head == 0) return 0;
    struct direntry *iter_entry = dir->head;
    if(iter_entry == 0) return 0;
    while(iter_entry != 0) {
        printf("%s\r\n", ((struct direntry*)iter_entry)->name);
        iter_entry = iter_entry->next;
    }
    return 0;
}

void register_filesystem(struct filesystem* fs, char *fs_name) {
  // register the file system to the kernel.
  // you can also initialize memory pool of the file system here.
    filesystem_array[0] = fs;
    filesystem_name[0]  = fs_name;
    return;
}

void init_root_filesystem() {
    // init vnode
    // mount the vnode 
    struct filesystem *root_fs = filesystem_array[0];
    rootfs_mount = (struct mount *)obj_allocate(sizeof(struct mount));
    root_fs->setup_mount(root_fs, rootfs_mount);
}

void vfs_ls(const char* pathname, int flags) {
    struct vnode *root_vnode = rootfs_mount->root;
    struct directory *start_dir = (struct directory *)root_vnode->internal;
    iterate_dir(start_dir);
}

struct file* vfs_open(const char* pathname, int flags) {
    struct vnode *root_vnode = rootfs_mount->root;
    struct file  *ret_file   = obj_allocate(sizeof(struct file)); // file descriptor
    struct vnode *find_vnode = 0;
    struct vnode *create_vnode = 0;

    if(root_vnode == 0) return 0;
    if((flags & REG_FILE) > 0) {
        // find the vnode
        root_vnode->v_ops->lookup(root_vnode, &find_vnode, pathname);
        if (find_vnode == 0 && (flags & O_CREAT) == 0) {
            
            // doesn't find the vnode
            return 0;
        }
        else if (find_vnode == 0 && (flags & O_CREAT) > 0) {
            root_vnode->v_ops->create(root_vnode, &create_vnode, pathname, REG_FILE);
            ret_file->f_pos = 0;
            ret_file->vnode = create_vnode;
        }
        else{
            ret_file->vnode = find_vnode;
            ret_file->f_pos = 0;
        }
        return ret_file;
    }
    return 0;
}

int vfs_close(struct file * file) {
    slab_put_obj(file);
    return 1;
}

int vfs_write(struct file* file, const void* buf, int len) {
    return file->vnode->f_ops->write(file, buf, len);
}

int vfs_read(struct file* file, void* buf, int len) {
    return file->vnode->f_ops->read(file, buf, len);
}

int user_open(const char* pathname, int flags) {
    // find a empty file descriptor in user task
    int i;
    struct file *file_descriptor;
    for(i = 0 ; i < MAX_FILE_NUM ; i++) {
        if(current->file_desp[i] == 0) {
            // allocate file descriptor
            current->file_desp[i] = vfs_open(pathname, flags);
            file_descriptor = current->file_desp[i];
            break;
        }
    }
    if(file_descriptor == 0) return -1;
    return i; 
}

int user_read(int file_index, void* buf, int len) {
    if(file_index >= MAX_FILE_NUM) return -1;
    struct file *file_descriptor;
    file_descriptor = current->file_desp[file_index];
    if(file_descriptor == 0) return -1;
    return vfs_read(file_descriptor, buf, len);
}

int user_write(int file_index, const void* buf, int len) {
    if(file_index >= MAX_FILE_NUM) return -1;
    struct file *file_descriptor;
    file_descriptor = current->file_desp[file_index];
    if(file_descriptor == 0) return -1;
    return vfs_write(file_descriptor, buf, len);
}

int user_close(int file_index) {
    if(file_index >= MAX_FILE_NUM) return -1;
    struct file *file_descriptor;
    file_descriptor = current->file_desp[file_index];
    if(file_descriptor == 0) return -1;
    vfs_close(file_descriptor);
    current->file_desp[file_index] = 0;
    return 0;
}

