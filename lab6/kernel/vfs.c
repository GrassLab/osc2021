#include "vfs.h"

static char pwd[MAX_PATH] = "/";
struct mount *rootfs;
struct filesystem_list *fs_list;

static int vfs_vnode_path(struct vnode *dir, char *buf){
    if(!dir || !dir->v_ops){
        return -1;
    }

    buf[0] = '\0';
    char component_name[MAX_PATH];
    while(1){
        component_name[0] = '\0';
        dir->v_ops->dir_name(dir, component_name);
        if(!strcmp(component_name, "/")){
            break;
        }
        strrev(component_name);
        strcat(buf, component_name);
        strcat(buf, "/");
        struct vnode *target;
        dir->v_ops->lookup(dir, &target, "..");
        dir = target->v_ops->content(target);
    }
    strrev(buf);
    return 0;
}

static struct vnode *vfs_find_vnode(const char *pathname, struct vnode *root){
    char path[MAX_PATH] = "";
    strncpy(path, pathname, -1);

    char *left = path;
    char *right = strtok(left, '/');
    left = right;
    struct vnode *dir = root;
    struct vnode *target;
    while(1){
        if(!dir || !dir->v_ops){
            return NULL;
        }

        right = strtok(left, '/');
        int ret = dir->v_ops->lookup(dir, &target, left);
        if(ret < 0){
            return NULL;
        }else if(ret == 1){
            target = target->v_ops->content(target);
        }else{
            if(target->mount){
                target = target->mount->root;
            }
        }

        if(!right){
            return target;
        }

        left = right;
        dir = target;
    }
}

int register_filesystem(struct filesystem *fs){
    struct filesystem_list *new = malloc(sizeof(struct filesystem_list));
    if(!new){
        return -1;
    }
    new->next = NULL;
    new->fs_mount = malloc(sizeof(struct mount));
    if(!new->fs_mount){
        free(new);
        return -1;
    }

    if(fs->setup_mount(fs, new->fs_mount) < 0){
        free(new->fs_mount);
        free(new);
        return -2;
    }

    if(!fs_list){
        fs_list = new;
    }else{
        struct filesystem_list *cur = fs_list;
        while(cur->next){
            cur = cur->next;
        }
        cur->next = new;
    }
    return 0;
}

struct file *fopen(const char *pathname, int flags){
    char path[MAX_PATH] = "";
    if(pathname[0] != '/'){
        strcat(path, get_pwd());
    }
    strcat(path, pathname);
    return vfs_fopen(path, flags, rootfs->root);
}

struct file *vfs_fopen(const char *pathname, int flags, struct vnode *root){
    char path[MAX_PATH] = "";
    if(pathname[0] != '/'){
        strcat(path, "/");
    }
    strcat(path, pathname);
    trim_multiple_of(path, '/');

    if(!strcmp(path, "/")){
        printf("vfs_fopen: cannot fopen '%s'!!!" NEW_LINE, pathname);
        return NULL;
    }

    size_t len = strlen(path) - 1;
    if(path[len] == '/'){
        path[len] = '\0';
    }

    struct vnode *dir = root;
    char *filename = split_last(path, '/');
    if(path[0] != '\0'){
        dir = vfs_find_vnode(path, root);
    }

    if(!dir || !dir->v_ops){
        printf("vfs_fopen: cannot fopen '%s'!!!" NEW_LINE, pathname);
        return NULL;
    }

    struct vnode *target;
    if(dir->v_ops->lookup(dir, &target, filename) < 0){
        target = NULL;
    }

    if(!target && !(flags & O_CREAT)){
        printf("vfs_fopen: cannot fopen '%s'!!!" NEW_LINE, pathname);
        return NULL;
    }

    struct file *fp = malloc(sizeof(struct file));
    if(!fp){
        printf("vfs_fopen: cannot fopen '%s'!!!" NEW_LINE, pathname);
        return NULL;
    }

    if(!target){
        if(dir->v_ops->create_file(dir, &target, filename) < 0){
            free(fp);
            printf("vfs_fopen: cannot fopen '%s'!!!" NEW_LINE, pathname);
            return NULL;
        }
    }

    if(!target->f_ops){
        printf("vfs_fopen: '%s' is not a file!!!" NEW_LINE, pathname);
        return NULL;
    }

    fp->vnode = target;
    fp->f_pos = 0;
    fp->f_ops = target->f_ops;
    fp->flags = flags;
    return fp;
}

int fclose(struct file *file){
    free(file);
    return 0;
}

int fwrite(struct file *file, const void *buf, size_t len){
    if(!file){
        return -1;
    }
    return file->f_ops->write(file, buf, len);
}

int fread(struct file *file, void *buf, size_t len){
    if(!file){
        return -1;
    }
    return file->f_ops->read(file, buf, len);
}

int fseek(struct file *fp, int offset, int whence){
    fp->f_pos = offset + whence;
    if(fp->f_pos >= fp->f_ops->filesize(fp)){
        if(fp->f_ops->filesize == 0){
            fp->f_pos = 0;
        }else{
            fp->f_pos = (fp->f_ops->filesize(fp) - 1);
        }
    }
    return 0;
}

int ftell(struct file *fp){
    return fp->f_pos;
}

void rewind(struct file *fp){
    fp->f_pos = 0;
}

int mkdir(const char *pathname){
    char path[MAX_PATH] = "";
    if(pathname[0] != '/'){
        strcat(path, get_pwd());
    }
    strcat(path, pathname);
    return vfs_mkdir(path, rootfs->root);
}

int vfs_mkdir(const char *pathname, struct vnode *root){
    char path[MAX_PATH] = "";
    if(pathname[0] != '/'){
        strcat(path, "/");
    }
    strcat(path, pathname);
    trim_multiple_of(path, '/');

    if(!strcmp(path, "/")){
        printf("vfs_mkdir: cannot mkdir '%s'!!!" NEW_LINE, pathname);
        return -1;
    }

    size_t len = strlen(path) - 1;
    if(path[len] == '/'){
        path[len] = '\0';
    }

    struct vnode *dir = root;
    char *dirname = split_last(path, '/');
    if(path[0] != '\0'){
        dir = vfs_find_vnode(path, root);
    }

    if(!dir || !dir->v_ops){
        printf("vfs_mkdir: cannot mkdir '%s'!!!" NEW_LINE, pathname);
        return -1;
    }

    return dir->v_ops->create_dir(dir, dirname);
}

int ls(const char *pathname){
    char path[MAX_PATH] = "";
    if(pathname[0] != '/'){
        strcat(path, get_pwd());
    }
    strcat(path, pathname);
    return vfs_ls(path, rootfs->root);
}

int vfs_ls(const char *pathname, struct vnode *root){
    char path[MAX_PATH] = "";
    if(pathname[0] != '/'){
        strcat(path, "/");
    }
    strcat(path, pathname);
    trim_multiple_of(path, '/');

    if(!strcmp(path, "/")){
        root->v_ops->list(root);
        return 0;
    }

    size_t len = strlen(path) - 1;
    if(path[len] == '/'){
        path[len] = '\0';
    }

    struct vnode *dir = vfs_find_vnode(path, root);

    if(!dir || !dir->v_ops){
        printf("vfs_ls: cannot ls '%s'!!!" NEW_LINE, pathname);
        return -1;
    }

    dir->v_ops->list(dir);
    return 0;
}

int cat(const char *pathname){
    char path[MAX_PATH] = "";
    if(pathname[0] != '/'){
        strcat(path, get_pwd());
    }
    strcat(path, pathname);
    return vfs_cat(path, rootfs->root);
}

int vfs_cat(const char *pathname, struct vnode *root){
    struct file *fp = vfs_fopen(pathname, 0, root);
    if(!fp){
        return -1;
    }
    char buffer[512];
    while(1){
        int n = fread(fp, buffer, sizeof(buffer));
        if(n == -22){
            break;
        }
        miniuart_send_nS(buffer, n);
    }
}

int vfs_mount(const char *device, const char *mountpoint, const char *filesystem){
    char path[MAX_PATH] = "";
    if(mountpoint[0] != '/'){
        strcat(path, "/");
    }
    strcat(path, mountpoint);
    trim_multiple_of(path, '/');

    if(!strcmp(path, "/")){
        if(rootfs){
            miniuart_send_S("vfs_mount: '/' is mounted!!!" NEW_LINE);
            return -1;
        }
        rootfs = get_mount(filesystem);
        if(rootfs){
            return 0;
        }
        printf("vfs_mount: fs '%s' not found!!!" NEW_LINE, filesystem);
        return -1;
    }

    size_t len = strlen(path) - 1;
    if(path[len] == '/'){
        path[len] = '\0';
    }

    struct vnode *dir = vfs_find_vnode(path, rootfs->root);

    if(!dir || !dir->v_ops){
        printf("vfs_mount: cannot mount at '%s'!!!" NEW_LINE, mountpoint);
        return -1;
    }

    if(dir->mount){
        printf("vfs_mount: cannot mount at '%s'!!!" NEW_LINE, mountpoint);
        return -1;
    }

    struct mount *mount = get_mount(filesystem);
    if(mount){
        dir->mount = mount;
        mount->root->v_ops->set_parent(mount->root, dir);
        return 0;
    }

    printf("vfs_mount: fs '%s' not found!!!" NEW_LINE, filesystem);
    return -1;
}

int vfs_umount(const char *mountpoint){
    if((mountpoint[0] == '/') && (mountpoint[1] == '\0')){
        miniuart_send_S("vfs_umount: cannot unmount '/'" NEW_LINE);
        return -1;
    }

    char *path = malloc(strlen(mountpoint) + 1);
    if(!path){
        return -1;
    }
    strncpy(path, mountpoint, strlen(mountpoint) + 1);

    char *left = path;
    char *right = strtok(left, '/');
    if(left[0] != '\0'){
        printf("vfs_umount: mountpoint '%s' must be absolute!!!" NEW_LINE, mountpoint);
        free(path);
        return -1;
    }
    char *pleft = left;
    left = right;
    struct vnode *dir = NULL;
    struct vnode *target = rootfs->root;
    while(1){
        if(!target){
            printf("vfs_umount: Missing directory '%s' in '%s'!!!" NEW_LINE, pleft, mountpoint);
            free(path);
            return -1;
        }
        dir = target;
        if(!dir || !dir->v_ops){
            printf("vfs_umount: '%s' in '%s' is not a directory!!!" NEW_LINE, pleft, mountpoint);
            free(path);
            return -1;
        }

        right = strtok(left, '/');
        if(dir->v_ops->lookup(dir, &target, left) < 0){
            target = NULL;
        }

        if(!right && target){
            break;
        }

        pleft = left;
        left = right;
    }

    if(!target->v_ops){
        printf("vfs_umount: '%s' is not a directory in '%s'!!!" NEW_LINE, left, mountpoint);
        free(path);
        return -1;
    }

    if(!target->mount){
        printf("vfs_umount: '%s' is not mounted!!!" NEW_LINE, left, mountpoint);
        free(path);
        return -1;
    }
    free(path);

    target->mount = NULL;
    return 0;
}

struct mount *get_mount(const char *filesystem){
    if(!fs_list){
        return NULL;
    }

    struct filesystem_list *cur = fs_list;
    while(cur){
        if(!strcmp(cur->fs_mount->fs->name, filesystem)){
            return cur->fs_mount;
        }
        cur = cur->next;
    }
    return NULL;
}

char *get_pwd(){
    Thread *t = current_thread();
    if(!t){
        return pwd;
    }
    return t->pwd;
}

int cd(const char *pathname){
    char path[MAX_PATH] = "";
    if(pathname[0] != '/'){
        strcat(path, "/");
    }
    strcat(path, pathname);
    trim_multiple_of(path, '/');

    if(!strcmp(path, "/")){
        strncpy(pwd, "/", -1);
        return 0;
    }

    struct vnode *dir = vfs_find_vnode(path, rootfs->root);

    if(!dir || !dir->v_ops){
        printf("cd: '%s' is not a directory!!!" NEW_LINE, pathname);
        return -1;
    }

    char buf[MAX_PATH];
    int ret = vfs_vnode_path(dir, buf);
    printf("\nret = %d, buf = %s\n", ret, buf);

    return 0;
}
