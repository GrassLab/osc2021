#include "vfs.h"
#include "tmpfs.h"
#include "printf.h"
#include "list.h"
#include "string.h"
#include "mm.h"

struct vnode_operations *tmpfs_v_ops = NULL;
struct file_operations *tmpfs_f_ops = NULL;

struct dentry* tmpfs_create_dentry(struct dentry *parent, const char *name, int type)
{
    printf("[tmpfs_create_dentry]\n");

    struct dentry *d = (struct dentry *) kmalloc(sizeof(struct dentry));
    
    strcpy(d->name, name);
    d->parent = parent;
    d->vnode = tmpfs_create_vnode(d, type);
    INIT_LIST_HEAD(&d->list);
    INIT_LIST_HEAD(&d->sub_dirs);
    // set parent's child list point to this new created dentry
    if (parent != NULL) {
        list_add(&d->list, &parent->sub_dirs);
    }
    
    d->mount = NULL;
    
    return d;
}

struct vnode* tmpfs_create_vnode(struct dentry *dentry, int type)
{
    printf("[tmpfs_create_vnode]\n");
    struct vnode *vnode = (struct vnode*) kmalloc(sizeof(struct vnode));
    vnode->dentry = dentry;
    vnode->v_type = type;
    vnode->v_ops = tmpfs_v_ops;
    vnode->f_ops = tmpfs_f_ops;
    vnode->internal = NULL;
    
    return vnode;
}

int tmpfs_register()
{
    if (tmpfs_v_ops != NULL && tmpfs_f_ops != NULL) {
        return TMPFS_ERROR;
    }

    // vnode operations
    tmpfs_v_ops = (struct vnode_operations *) kmalloc(sizeof(struct vnode_operations));
    tmpfs_v_ops->lookup = tmpfs_lookup;
    tmpfs_v_ops->create = tmpfs_create;
    tmpfs_v_ops->mkdir = tmpfs_mkdir;

    // file operations
    tmpfs_f_ops = (struct file_operations *) kmalloc(sizeof(struct file_operations));
    tmpfs_f_ops->write = tmpfs_write;
    tmpfs_f_ops->read = tmpfs_read;
    // Register error
    return 0;
}

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount, const char *component_name)
{
    printf("[tmpfs_setup_mount]\n");
    mount->fs = fs;
    mount->root = tmpfs_create_dentry(NULL, component_name, DIRECTORY);
    
    printf("[tmpfs_setup_mount] New created dentry name = %s\n", mount->root->name);

    return TMPFS_ERROR;
}

int tmpfs_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    #ifdef __FS_DEBUG
    printf("[tmpfs_lookup]\n");
    #endif //__DEBUG

    int isNextVnodeFound = FALSE;
    struct dentry *dir_dentry = dir_node->dentry;
    struct dentry *pos;
    
    list_for_each_entry(pos, &dir_dentry->sub_dirs, list) {
        #ifdef __FS_DEBUG
        printf("[tmpfs_lookup] pos->name = %s\n", pos->name);
        #endif //__DEBUG
        if (!strcmp(pos->name, component_name)) {
            *target = pos->vnode;
            isNextVnodeFound = TRUE;
            break;
        }
    }

    return isNextVnodeFound;
}

int tmpfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    struct dentry *temp = tmpfs_create_dentry(dir_node->dentry, component_name, REGULAR_FILE);
    *target = temp->vnode;
    return 0;
}

/**
 * 
 */
int tmpfs_write(struct file *file, const void *buf, size_t len)
{
    #ifdef __FS_DEBUG
    printf("[tmpfs_write] Write content in buf : %s\n", buf);
    printf("[tmpfs_write] vnode address = 0x%x\n", file->vnode);
    #endif //__DEBUG

    struct tmpfs_internal *m_tmpfs_internal;
    // file no allocated moemory
    if (file->nr_internal_moemory_page_allocated == 0) {
        m_tmpfs_internal = (struct tmpfs_internal *) kmalloc(TMP_DEFAULT_FILE_SIZE);
        m_tmpfs_internal->max_buf_size = TMP_DEFAULT_FILE_SIZE - sizeof(typeof(m_tmpfs_internal->max_buf_size));
        m_tmpfs_internal->buf = (char *) kmalloc(TMP_DEFAULT_FILE_SIZE);
        file->vnode->internal = m_tmpfs_internal;

        file->nr_internal_moemory_page_allocated++;
    }

    // write overflow checking
    if (len + file->f_pos > m_tmpfs_internal->max_buf_size) {
        printf("[tmpfs_write] Error, Buffer will be overflow after written!");
        return TMPFS_WRITE_OVERFLOW_ERROR; // Error code
    }

    char *dest = &m_tmpfs_internal->buf[file->f_pos];
    char *src = (char *)buf;
    // 1. write len byte from buf to the opened file.
    int i;
    for (i = 0;i < len;i++) {
        dest[i] = src[i];
    }
    dest[i] = EOF;

    // Update file f_pos
    file->f_pos += len;
    printf("[tmpfs_write] file->f_pos = %d\n", file->f_pos);
    // 2. return written size or error code if an error occurs.
    return len;
}

int tmpfs_read(struct file *file, void *buf, size_t len)
{
    #ifdef __FS_DEBUG
    printf("[tmpfs_read] vnode address = 0x%x\n", file->vnode);
    printf("[tmpfs_read] file->f_pos = %d\n", file->f_pos);
    #endif //__DEBUG

    struct tmpfs_internal *m_tmpfs_internal = file->vnode->internal;
    char *src = &m_tmpfs_internal->buf[file->f_pos];
    char *dest = (char *)buf;
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    int i;
    for (i = 0;i < len && src[i] != (uint8_t)EOF;i++) {
        dest[i] = src[i];

        #ifdef __FS_DEBUG
        printf("[tmpfs_read] src[i] = %c\n", src[i]);
        #endif //__DEBUG
    }
    
    // Update file f_pos with min(len, readable file data size) byte
    file->f_pos += i;

    // 2. return read size or error code if an error occurs.
    return i;
}

int tmpfs_mkdir(struct vnode *parent, const char *component_name)
{
    printf("[tmpfs_mkdir]\n");
    tmpfs_create_dentry(parent->dentry, component_name, DIRECTORY);

    return 1;
}
