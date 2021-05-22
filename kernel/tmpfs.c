#include "allocator.h"
#include "vfs.h"
#include "uart.h"
#include "cpio.h"
#include "tmpfs.h"
#include "string.h"

void print_tmpfsTree(vnode *root) {
    iContent *content = (iContent*)root->internal;
    if(content->type == DIR_TYPE) {
        uart_puts(content->name);
        uart_puts("\n");
        vnode **childern = (vnode**)content->data;
        for(int i = 0; i < content->size; i++) {
            uart_puts("|--");
            print_tmpfsTree(childern[i]);
        }
    }
    else if(content->type == FILE_TYPE) {
        uart_puts(content->name);
        uart_puts(", size = ");
        uart_puts_int(content->size);
        uart_puts(" bytes, ");
        uart_puts_int(content->type);
        uart_puts(" type\n");
    }
}

int tmpfs_write(file* file, const void* buf, unsigned long len) {
    vnode *node = file->vnode;
    iContent *content = (iContent*)node->internal;

    const char *buffer = (const char*) buf;
    if(!content->data) {
        content->data = kmalloc(sizeof(100));
    }
    char *data = (char*)content->data;
    if(content->type == FILE_TYPE) {
        if((file->f_pos)+len < content->capacity) {
            int j = file->f_pos;
            for(int i = 0; i < len; i++) {
                data[j++] = buffer[i];
            }
            return len;
        }
        
        else {
            int j = file->f_pos;
            for(int i = 0; i < len; i++) {
                data[j++] = buffer[i];
            }
            content->capacity = j;
            content->size = j;
            return j;
        }
    }
    return 0;
}

int tmpfs_read(file* file, void* buf, unsigned long len) {
    vnode *node = file->vnode;
    iContent *content = (iContent*)node->internal;
    int fsize = content->size;
    int read_len = len < fsize ? len : fsize;

    int j = 0;
    char *buffer = (char*) buf;
    char *data = (char*)content->data;
    for(int i = file->f_pos; i < read_len; i++) {
        buffer[j++] = data[i];
    }
    file->f_pos += read_len;
    
    return j;
}

/* create a new child under dir_node*/
int tmpfs_create(vnode* dir_node, vnode** target, const char* component_name) {
    iContent *content = (iContent*) dir_node->internal;
    if(content->type != DIR_TYPE) {
        uart_puts("tmpfs_create(): only accept DIR_TYPE!\n");
    }

    vnode **childern = (vnode**) content->data;
    int index = -1;
    if(content->size < content->capacity) {
        index = content->size++;
    }
    else {
        uart_puts("tmpfs_create(): dir_capacity is not enough!\n");
        while(1);
    }

    vnode* child = (vnode*)kmalloc(sizeof(vnode));
    child->mount = dir_node->mount;
    child->v_ops = dir_node->v_ops;
    child->f_ops = dir_node->f_ops;
    child->internal = (iContent*)kmalloc(sizeof(iContent));
    content = child->internal;
    content->name = (char*)component_name;
    content->type = FILE_TYPE;
    content->capacity = 0;
    content->size = 0;
    content->data = 0;

    childern[index] = child;
    if(target) {
        *target = child;
    }
    
    return index;
}

int tmpfs_lookup(vnode* dir_node, vnode** target, const char* component_name) {
    iContent *content = (iContent*) dir_node->internal;
    if(content->type != DIR_TYPE) {
        uart_puts("tmpfs_lookup(): only accept DIR_TYPE!\n");
    }

    vnode **childern = (vnode**) content->data;
    for(int i = 0; i < content->size; i++) {
        vnode *child = childern[i];
        iContent *child_content = (iContent*)child->internal;
        if(!strcmp(child_content->name, component_name)) {
            if(target) {
                *target = child;
            }
            return i; // child index
        }
    } 
    return -1; // child is not exsited, should create it
}

int tmpfs_init(mount *mnt, vnode *root) {
    root->mount = mnt;
    root->v_ops = (vnode_operations*) kmalloc(sizeof(vnode_operations));
    root->v_ops->lookup = tmpfs_lookup;
    root->v_ops->create = tmpfs_create;
    root->f_ops = (file_operations*) kmalloc(sizeof(file_operations));
    root->f_ops->write = tmpfs_write;
    root->f_ops->read = tmpfs_read;
    root->internal = (void*)kmalloc(sizeof(iContent));

    iContent *content = (iContent*)root->internal;
    content->name = 0;
    content->type = DIR_TYPE;
    content->capacity = DIR_CAP;
    content->size = 0;
    content->data = (void*)kmalloc(DIR_CAP*8);
    
    void *cpio_base = get_cpio_base();
    while(1) {
        f_prop* f = get_file_property(cpio_base);
        if(!strcmp(f->fname, "TRAILER!!!")) {
            break;
        }
        vnode *dir_node = root;
        content = (iContent*)(dir_node->internal);
        vnode **target = (vnode**)(content->data);
        while(1) {
            int index = tmpfs_lookup(dir_node, 0, f->fname);
            if(index == -1) {
                index = tmpfs_create(dir_node, 0, f->fname);
                vnode *child = target[index];
                iContent* child_content = (iContent*)child->internal;
                // update the content accroding to f->fmode
                if(f->fmode == 4) { // directory
                    child_content->capacity = DIR_CAP;
                    child_content->size = 0;
                    child_content->data = (void*)kmalloc(DIR_CAP*8);
                }
                else if(f->fmode == 8) { // regular file
                    child_content->size = f->fsize;
                    child_content->data = f->fdata;
                    child_content->capacity = f->fsize;
                }
                else {
                    uart_puts("tmpfs_init(): unknown type!\n");
                }
                break;
            }
        }

        cpio_base += 110;
        cpio_base += f->fsize;
        cpio_base += f->nsize;
        
    }
    print_tmpfsTree(root);
    return 1;
}

int tmpfs_setup(filesystem *fs, mount *mnt) {
    mnt->root = (vnode*)kmalloc(sizeof(vnode));
    mnt->fs = fs;

    if(!tmpfs_init(mnt, mnt->root)) {
        uart_puts("tmpfs_rootInit error!\n");
        while(1);
    }
    return 0;
}