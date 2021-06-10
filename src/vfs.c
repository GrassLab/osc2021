#include "vfs.h"
#include "tmpfs.h"
#include "fat.h"
#include "mm.h"
#include "utils.h"
#include "uart.h"

mount_t *rootfs;

static void vfs_list_dir(list_head_t *dir, file_type_t type) {
    list_head_t *pos = dir->next;
    while (pos != dir) {
        vnode_t *tmp;
        if (type == DIR) {
            tmp = list_entry(pos, vnode_t, dentry.sibling);
        } else {
            tmp = list_entry(pos, vnode_t, node);
        }
        async_write(tmp->name, strlen(tmp->name));
        if (type == DIR)
            async_uart_putc('/');
        async_write("\n", 1);
        pos = pos->next;
    }
}

static int vfs_lookup(vnode_t *dir_node, vnode_t **target, const char *component_name,
                      char **target_name, file_type_t type) {
    if (*component_name == '/') {
        dir_node = rootfs->root;
        component_name++;
    }
    /* Parse name */
    list_head_t *pos;
    int shift;
    /* Search dir */
    while ((shift = strchr(component_name, '/')) >= 0 || type == DIR) {
        char name[MAX_NAME_SIZE];
        uint8_t find = 0;
        if (shift >= 0) {
            strncpy(name, component_name, shift);
            component_name += (shift + 1);
        } else {
            strcpy(name, component_name);
        }

        if (!strcmp(name, ".") || !strcmp(name, "..")) {
            if (!strcmp(name, "..")) {
                if (dir_node->dentry.parent != NULL) {
                    dir_node = dir_node->dentry.parent;
                    find = 1;
                }
            } else {
                find = 1;
            }
        } else {
            pos = dir_node->dentry.subdir.next;
            while (pos != &dir_node->dentry.subdir) {
                vnode_t *tmp = list_entry(pos, vnode_t, dentry.sibling);
                if (!strcmp(name, tmp->name)) {
                    find = 1;
                    if (tmp->dentry.mount == NULL) {
                        dir_node = tmp;
                    } else {
                        dir_node = tmp->dentry.mount->root;
                    }
                    break;
                }
                pos = pos->next;
            }
        }

        /* shift < 0 -> last component */
        if (!find) {
            if (shift < 0) {
                *(target) = dir_node;
                *(target_name) = component_name;
            }
            return -1;
        } else if (find && shift < 0) {
            *(target) = dir_node;
            return 0;
        }
    }
    /* Search file */
    pos = dir_node->node.next;
    *(target_name) = component_name;
    while (pos !=  &dir_node->node) {
        vnode_t *tmp = list_entry(pos, vnode_t, node);
        if (!strcmp(component_name, tmp->name)) {
            *(target) = tmp;
            return 0;
        }
        pos = pos->next;
    }
    *(target) = dir_node;
    return -1;
}

static void vfs_traverse_unmount(vnode_t *root, void(*umount_ops)(vnode_t*)) {
    list_head_t *pos = root->dentry.subdir.next;
    while(pos != &root->dentry.subdir) {
        vnode_t *tmp = list_entry(pos, vnode_t, dentry.sibling);
        vfs_traverse_unmount(tmp, umount_ops);
        pos = root->dentry.subdir.next;
    }

    pos = root->node.next;
    while (pos != &root->node) {
        list_del(pos);
        vnode_t *file = list_entry(pos, vnode_t, node);
        umount_ops(file);
        kfree(file);
        pos = root->node.next;
    }
    list_del(&root->dentry.sibling);
    kfree(root);
    return ;
}

static int vfs_umount(vnode_t **dir_node, const char *pathname) {
    char *dir_name;
    vnode_t *target = NULL;
    vnode_t *dir = *dir_node;
    if (vfs_lookup(dir, &target, pathname, &dir_name, DIR) >= 0) {
        if (target->dentry.prev != NULL) {
            dir = target->dentry.prev;
            vfs_traverse_unmount(target, dir->dentry.mount->umount_ops);
            kfree(dir->dentry.mount->fs_name);
            kfree(dir->dentry.mount);
            dir->dentry.mount = NULL;
            return 0;
        }
    }
    return -1;
}

static int vfs_chdir(vnode_t **dir_node, const char *pathname) {
    char *dir_name;
    vnode_t *target = NULL;
    vnode_t *dir = *dir_node;
    if (vfs_lookup(dir, &target, pathname, &dir_name, DIR) >= 0) {
        *dir_node = target;
        return 0;
    }
    return -1;
}

static int vfs_mkdir(vnode_t **dir_node, const char *pathname) {
    char *dir_name;
    vnode_t *target = NULL;
    vnode_t *dir = *dir_node;
    if (vfs_lookup(dir, &target, pathname, &dir_name, DIR) < 0) {
        if (target != NULL) {
            dir = target;
            dir->v_ops->create_dir(dir, dir_name);
            return 0;
        }
    }
    return -1;
}

static int vfs_ls(vnode_t **dir_node, const char *pathname) {
    char *dir_name;
    vnode_t *target = NULL;
    vnode_t *dir = *dir_node;
    if (vfs_lookup(dir, &target, pathname, &dir_name, DIR) >= 0) {
        vfs_list_dir(&target->dentry.subdir, DIR);
        vfs_list_dir(&target->node, FILE);
        return 0;
    }
    return -1;
}

int vfs_dir_operation(const char *pathname, vnode_t **dir_node, uint16_t mode) {
    if (!strcmp(pathname, "\0") || pathname == NULL)
        return -1;
    uint32_t len = strlen(pathname);
    char *p_name = kmalloc(len + 1);
    strcpy(p_name, pathname);
    if (*(p_name + len - 1) == '/')
        *(p_name + len - 1) = (char)0;
    /*
    /   Mode 0 for system call -> ls
    /   Mode 1 for system call -> mkdir
    /   Mode 2 for system call -> chdir
    /   Mode 3 for system call -> umount
    */
    int result;
    if (!mode) {
        result = vfs_ls(dir_node, p_name);
    } else if (mode == 1) {
        result = vfs_mkdir(dir_node, p_name);
    } else if (mode == 2) {
        result = vfs_chdir(dir_node, p_name);
    } else if (mode == 3) {
        result = vfs_umount(dir_node, p_name);
    }
    kfree(p_name);
    return result;
}

int vfs_mount(const char *device, const char *pathname,
              const char *filesystem, vnode_t **dir_node) {
    if (!strcmp(pathname, "\0") || pathname == NULL)
        return -1;
    uint32_t len = strlen(pathname);
    char *p_name = kmalloc(len + 1);
    strcpy(p_name, pathname);
    char *dir_name;
    vnode_t *target = NULL;
    vnode_t *dir = *dir_node;
    if (*(p_name + len - 1) == '/')
        *(p_name + len - 1) = (char)0;
    if (vfs_lookup(dir, &target, pathname, &dir_name, DIR) >= 0) {
        if (!vfs_setup_mount(&target->dentry.mount, device, filesystem)) {
            vnode_t *root = target->dentry.mount->root;
            root->dentry.prev = target;
            root->dentry.parent = target->dentry.parent;
            if (dir == target)
                *dir_node = root;
            kfree(p_name);
            return 0;
        }
    }
    kfree(p_name);
    return -1;
}

int vfs_read(file_t *file, void *buf, size_t len) {
    if (file == NULL)
        return -1;
    if (!len)
        return 0;
    return file->vnode->f_ops->read(file, buf, len);
}

int vfs_write(file_t *file, const void *buf, size_t len) {
    if (file == NULL)
        return -1;
    if (!len)
        return 0;
    return file->vnode->f_ops->write(file, buf, len);
}

int vfs_close(file_t *file) {
    if (file == NULL)
        return -1;
    if (!(--file->use_num))
        kfree(file);
    return 0;
}

file_t* vfs_open(const char *pathname, int flags, vnode_t *dir_node) {
    if (!strcmp(pathname, "\0") || pathname == NULL)
        return NULL;
    char *file_name = NULL;
    vnode_t *target;
    if (vfs_lookup(dir_node, &target, pathname, &file_name, FILE) < 0) {
        if (flags) {
            dir_node = target;
            dir_node->v_ops->create_file(dir_node, &target, file_name);
        } else {
              return NULL;
        }
    }
    /* File structure initialization */
    file_t *file = kmalloc(sizeof(file_t));
    file->vnode = target;
    file->f_pos = 0;
    file->use_num = 1;
    return file;
}

int vfs_setup_mount(mount_t **mount, const char *device, const char *filesystem) {
    if (!strcmp(filesystem, "tmpfs")) {
        *mount = kmalloc(sizeof(mount_t));
        tmpfs_setup_mount(*mount, device);
        return 0;
    } else if (!strcmp(filesystem, "fat")) {
        *mount = kmalloc(sizeof(mount_t));
        if (fat_set_mount(*mount, device) < 0) {
            kfree(mount);
            return -1;
        } else {
            return 0;
        }
    }
    return -1;
}

void vfs_vnode_init(vnode_t *vnode, const char *name) {
    strcpy(vnode->name, name);
    vnode->dentry.mount = NULL;
    vnode->dentry.prev = NULL;
    vnode->dentry.parent = NULL;
    list_init(&vnode->dentry.sibling);
    list_init(&vnode->dentry.subdir);
    vnode->v_ops = NULL;
    vnode->f_ops = NULL;
    vnode->internal = NULL;
    list_init(&vnode->node);
}

void vfs_init(const char *filesystem) {
    tmpfs_init();
    fat_init();
    vfs_setup_mount(&rootfs, filesystem, filesystem);
    /* Populate with initramfs */
    vnode_t *root = rootfs->root;
    for (int i = 0; i < file_count; i++) {
        if (file_list[i].file_type == FILE) {
            file_t *file = vfs_open(file_list[i].file_name, 1, root);
            vfs_write(file, file_list[i].file_content, file_list[i].file_size);
            kfree(file);
        } else {
            vfs_dir_operation(file_list[i].file_name, &root, 1);
        }
    }
}
