#include "../include/vfs.h"
#include "../include/initrd.h"
#include "../include/stringUtils.h"
#include "../include/memAlloc.h"
#include "../include/tmpfs.h"



file* vfsOpen(const char* pathname,int flags ){
    vnode* dir_node = my_mount.root;
    vnode* child;
    while(1){
        char path[50];
        pathname = slashIgnore(pathname,path);
        int idx = dir_node->v_ops->lookup(dir_node,&child,path);

        if(pathname){
            if(idx>=0){
                dir_node = child;
            }else{
                uart_printf("no such dir\n");
            }
        }else{
            if(idx>=0){
                break;//v_node exist
            }else{
                dir_node->v_ops->create(dir_node, &child, path);
                break;
            }
        }
    }
    
    file* new_file = (file*)my_alloc(sizeof(file));
    new_file->v_node = child;
    //uart_printf("open vnode :%d\n",new_file->v_node);
    new_file->f_pos = 0;
    new_file->f_ops = (file_operations*)(my_alloc(sizeof(file_operations)));
    new_file->f_ops->write = my_write_f;
    new_file->f_ops->read = my_read_f;
    new_file->flags = flags;
    return new_file;
}

int vfsWrite(file* f, const void* buf, int length){
    return f->f_ops->write(f,buf,length);
}

int vfsRead(file* f, void* buf, int length){
    return f->f_ops->read(f,buf,length);
}

int vfsClose(file* f){
    my_free(f->f_ops);
    my_free(f);
    return 0;
}
int vfsInit(void *setup_mount_f){
    int (*setup_mount)(filesystem* fs,mount* mnt) = setup_mount_f;

    //my_write_f = write_f;
    //my_read_f = read_f;
    setup_mount(&my_filesystem,&my_mount);
    uart_printf("%s fs has been setup\n",my_filesystem.name);
}

