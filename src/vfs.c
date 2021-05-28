#include "vfs.h"
#include "fs.h"
#include "mm.h"
#include "printf.h"
#include "string.h"
#include "tmpfs.h"
#include "sys.h"
#include "utils.h"
#include "sched.h"
#include "cpio.h"
#include "fat32.h"

struct mount *rootfs;

void rootfs_init()
{
    // register the root file system to kernel 
    register_filesystem(&tmpfs);

    // mount root file system
    rootfs = (struct mount *) kmalloc(sizeof(struct mount));
    tmpfs.setup_mount(&tmpfs, rootfs, "/");
}

int register_filesystem(struct filesystem* fs) {

    // register the file system to the kernel.
    if (!strcmp(fs->name, "tmpfs")) {
        int res = tmpfs_register();
        if (res == TMPFS_ERROR) {
            printf("[register_filesystem] Register tmpfs error");
            return VFS_ERROR;
        }

        printf("[register_filesystem] Register tmpfs filesystem\n");
    }

    if (!strcmp(fs->name, "fat32")) {
        int res = fat32_register();
        if (res == TMPFS_ERROR) {
            printf("[register_filesystem] Register fat32 error");
            return VFS_ERROR;
        }

        printf("[register_filesystem] Register fat32 filesystem\n");
    }

  return 0;
}

struct file *create_fd(struct vnode *target_file)
{
    struct file *fd = (struct file *) kmalloc(sizeof(struct file));
    fd->vnode = target_file;
    fd->f_pos = 0;
    fd->f_ops = target_file->f_ops;
    fd->flags = 0;
    fd->nr_internal_moemory_page_allocated = 0;

    #ifdef __FS_DEBUG
    printf("[create_fd] fd = 0x%x\n", fd);
    _vfs_dump_dentry(target_file->dentry);
    #endif //__DEBUG

    return fd;
}

void _finalComponentName(const char *pathname, char *target_component_name)
{
    int s_len = strlen(pathname);
    int idx_scan = 1;

    // skip '/' at the tail of pathname
    while (pathname[s_len - idx_scan] == '/') {
        idx_scan++;
    }

    int i = 0;
    // read from back to front idx until ecounter first "/" or the beginning of the pathname
    while (pathname[s_len - idx_scan] != '/' && idx_scan <= s_len) {
        target_component_name[i++] = pathname[s_len - idx_scan];
        idx_scan++;
    }
    target_component_name[i] = '\0';
    reverse(target_component_name);

}

int _vnode_path_traversal(struct vnode *rootnode, const char *pathname, struct vnode **target_file, char *target_component_name)
{
    // TODO: explain how this function work, this function is most important but quite complex
    int path_idx = 0;
    int isNextVnodeFound = 0;
    char temp_component_name[DNAME_INLINE_LEN];
    struct vnode *temp_found_file; // temporal component name when traversal pathname
    *target_file = rootnode;       // target file will be root vnode
    
    // If pathname is absolute path -> first char of pathname is '/'
    if (pathname[0] == '/') {
        // edge case pathname contain single '/'
        if (strlen(pathname) == 1) return TRUE;
        path_idx++; // skip first '/'
    }

    // read target_component_name from pathname
    _finalComponentName(pathname, target_component_name);
    printf("[_vnode_path_traversal] target_component_name = %s\n", target_component_name);
    while (1) {
        // Extract target_component_name until delimit
        // And finally target_component_name will be component name of the target file
        int i = 0;
        while (pathname[path_idx] != '/' && pathname[path_idx] != '\0') {
            temp_component_name[i++] = pathname[path_idx++];
        }
        temp_component_name[i] = '\0';
        path_idx++; // for skip '/'
        // printf("[_vnode_path_traversal] path_idx(end idx) = %d, component name= %s\n", path_idx, temp_component_name);

        if (!strcmp(temp_component_name, "\0"))
        {
            // reach '\0' will be here, represent we find out the target file
            //printf("[_vnode_path_traversal] !strcmp(temp_component_name, '')!!!!!!!!!\n");
            return TRUE; // 
        }
        if (!strcmp(temp_component_name, "."))
        {
            //printf("[_vnode_path_traversal] !strcmp(temp_component_name, '.')!!!!!!!!!\n");
            continue;
        }
        if (!strcmp(temp_component_name, ".."))
        {
            //printf("[_vnode_path_traversal] !strcmp(temp_component_name, '..')!!!!!!!!!\n");
            
            if (rootnode->dentry->parent == NULL) // if rootnode is root of file system 
                return VFS_NOT_VALID_PATH_ERROR;
            
            // If this directory is mounted, should go to the parent directory vnode of the mountpoint. On the other word,
            // go back to original directory vnode(Cross the Mountpoint).
            if (rootnode->dentry->parent->mount != NULL) {
                printf("[_vnode_path_traversal] .. Cross the mountpoint, rootnode->dentry->parent->name = %s!\n", rootnode->dentry->parent->name);
                rootnode = rootnode->dentry->parent->vnode;
            }

            rootnode = rootnode->dentry->parent->vnode;
            *target_file = rootnode;
            printf("(*target_file)->dentry->name : %s\n", (*target_file)->dentry->name);
            continue;
        }

        // Find extracted component name in vnode's subdirectory(child)
        // Then check if search sucessfully or not
        isNextVnodeFound = rootnode->v_ops->lookup(rootnode, &temp_found_file, temp_component_name);
        if (isNextVnodeFound) { 
            // If this directory mounted, should go to the mounted file system’s root directory vnode(cross the mountpoint)
            // instead of the original vnode.
            if (temp_found_file->dentry->mount != NULL) {
                printf("[_vnode_path_traversal] Cross the mountpoint!\n");
                temp_found_file = temp_found_file->dentry->mount->root->vnode;
            }

            *target_file = temp_found_file; // update target_file each time if found next Vnode found
            if (!strcmp(temp_component_name, target_component_name)) {
                // find out target file
                *target_file = temp_found_file;
                return TRUE;
            }
            else {
                // continue traversal next compnent name in pathname
                rootnode = temp_found_file;
            }
        }
        else { // next vnode not found
            if (!strcmp(temp_component_name, target_component_name)) {
                // Traversal reach the target component name but vnode not exist. 
                // So we need to return previous found vnode (vnode of previous component name of target component name)  
                // for support create new file(if O_CREAT flag is set to vfs_open()).
                // Because in previous loop target_file have been updated to newest(prvious vnode), we
                // can do nothing here
                break;
            }
            else {
                // not valid path, terminate pathname traversal
                return VFS_NOT_VALID_PATH_ERROR;
            }
        }
    }

    return FALSE;
}

int _lookUp_pathname(const char *pathname, struct vnode **target_file, char *target_component_name)
{
    struct vnode *rootnode;
    int loopUp_result;

    if (pathname[0] == '/') {
        // absolute path, lookup pathname from the root vnode
        rootnode = rootfs->root->vnode;
    } else {
        // relative path, lookup pathname from the current task vnode
        rootnode = current->cwd->vnode; 
    }

    loopUp_result = _vnode_path_traversal(rootnode, pathname, target_file, target_component_name);

    if (loopUp_result)
        printf("[_lookUp_pathname] Found Target: (*target_file)->dentry->name: %s\n", (*target_file)->dentry->name);
    else 
        printf("[_lookUp_pathname] Parent: (*target_file)->dentry->name: %s\n", (*target_file)->dentry->name);
    
    return loopUp_result; 
    
}

struct file* vfs_open(const char *pathname, int flags) {
    if (flags == O_CREAT) {
        printf("\n==========> vfs_open(%s, %s) <==========\n", pathname, "O_CREAT");
    } else {
        printf("\n==========> vfs_open(%s, %s) <==========\n", pathname, "0");
    }

    struct vnode *target_file = NULL;  // The vnode of target file, the target file may be either regular file or directory
    char target_component_name[DNAME_INLINE_LEN]; // The component name of target file

    // 1. Find target_file node and target_component_name based on pathname. Lookup pathname.
    int loopUp_result = _lookUp_pathname(pathname, &target_file, target_component_name);

    if (loopUp_result == VFS_NOT_VALID_PATH_ERROR) {
        printf("Path %s not exsit!\n", pathname);
    }

    // 2. Create a new file descriptor for this vnode if found.
    if (loopUp_result == TRUE) {
        printf("[vfs_open] Create a new file descriptor for this vnode, vnode name: %s\n", target_file->dentry->name);
        struct file *fd = create_fd(target_file);
        return fd;
    } else { 
        // 3. Create a new file if O_CREAT is specified in flags.
        if (flags == O_CREAT) { 
            struct vnode *created_file;
            target_file->v_ops->create(target_file, &created_file, target_component_name);
            struct file *fd = create_fd(created_file);
            return fd;
        } else {
            printf("[vfs_open] 3. flags not O_CREAT, vfs_open do nothing, return NULL\n");
        }
    }
    return NULL;
}

int vfs_close(struct file* file) {
    // 1. release the file descriptor
    kfree(file);
    return 0;
}

int vfs_write(struct file* file, const void* buf, size_t len) {
     if (file->vnode->v_type != REGULAR_FILE){
        printf("[vfs_write] Error, Write a non regular file\n");
        return VFS_WRITE_FILE_ERROR;
    } 

    // 1. write len byte from buf to the opened file.
    int nr_byte_written = file->f_ops->write(file, buf, len);

    // 2. return written size or error code if an error occurs.
    if (nr_byte_written < 0) return VFS_WRITE_FILE_ERROR;
    return nr_byte_written;
}

int vfs_read(struct file *file, void *buf, size_t len) {
    if (file->vnode->v_type != REGULAR_FILE){
        printf("Read a non regular file\n");
        return VFS_READ_FILE_ERROR;
    } 
    
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    int nr_byte_read = file->f_ops->read(file, buf, len);
    
    // 2. return read size or error code if an error occurs.
    if (nr_byte_read < 0) return VFS_READ_FILE_ERROR;
    return nr_byte_read;
}


void vfs_populate_initramfs()
{
    const char *initramfs_path = "/initramfs/";
    int initramfs_path_len = strlen(initramfs_path);
    vfs_mkdir(initramfs_path);

    struct cpio_header *header = (void *) INITRAMFS_ADDR;

    const char *current_filename;
    struct cpio_header *next;
    void *result;
    int error;
    unsigned long size;
    
    struct file *cpio_file;
    while (1) {
        error = cpio_parse_header(header, &current_filename, &size,
                &result, &next);
        // Break on an error or nothing left to read.
        if (error) {
            break;
        }

        // printf("current_filename = %s \n", current_filename);
        char pathname[DNAME_INLINE_LEN];
        strcpy(pathname, initramfs_path);
        strcpy(pathname+initramfs_path_len, current_filename);
        printf("[vfs_populate_initramfs] pathname = %s\n", pathname);

        cpio_file = vfs_open(pathname, O_CREAT);
        
        vfs_write(cpio_file, result, size);
        vfs_close(cpio_file);

        header = next;
    }
}

// For easy, we just return 1-d array to buffer all filename
char *vfs_read_directory(struct file *file) 
{
    struct dentry *dir_dentry = file->vnode->dentry;
    struct dentry *pos;
    
    struct list_head *p;
    int counter = 0;
    list_for_each(p, &dir_dentry->sub_dirs) {
        counter++;
    }

    printf("[vfs_read_directory] The number of file in %s directory : %d\n", dir_dentry->name, counter);

    // Here assume max file path is (DNAME_INLINE_LEN * 2 + 1), +1 for '\0'
    char *buf_filenames = kmalloc(counter * (DNAME_INLINE_LEN * 2 + 1) * sizeof(char));

    int buf_idx = 0;
    list_for_each_entry(pos, &dir_dentry->sub_dirs, list) {
        //printf("[vfs_ls] - %s%s\n", dir_dentry->name, pos->name);
        
        // copy path (exclude file name)
        // strcpy(&buf_filenames[buf_idx], dir_dentry->name);
        // buf_idx += strlen(dir_dentry->name);

        // copy filename
        strcpy(&buf_filenames[buf_idx], pos->name);
        buf_idx += strlen(pos->name) + 1; // +1 for '\0' added in strcpy()
        

        // for (int i = 0;i < buf_idx;i++) {
        //     if (buf_filenames[i] == '\0')
        //         printf("?");
        //     printf("%c", buf_filenames[i]);
        // }
        // printf("\n");
    }
    buf_filenames[buf_idx] = EOF; // end of buf, like read file

    // int buf_idx_test = 0;
    // while (buf_filenames[buf_idx_test] != (uint8_t)EOF) {
    //     printf("buf_filenames = %s  ", buf_filenames + buf_idx_test);
    //     buf_idx_test += strlen(buf_filenames + buf_idx_test) + 1;
    // }

    return buf_filenames;
}

int vfs_mkdir(const char *pathname)
{
    struct vnode *target_file;
    char target_component_name[DNAME_INLINE_LEN]; // The component name of target file
    if (_lookUp_pathname(pathname, &target_file, target_component_name) == VFS_NOT_VALID_PATH_ERROR) {
        printf("[vfs_mkdir] Path %s not exsit!\n", pathname);
        return VFS_NOT_VALID_PATH_ERROR;
    }

    int res = target_file->v_ops->mkdir(target_file, target_component_name);
    return res;
}

// change directory for current process
int vfs_chdir(const char *pathname)
{
    struct vnode *target_file;
    char target_component_name[DNAME_INLINE_LEN]; // The component name of target file
    if (_lookUp_pathname(pathname, &target_file, target_component_name) == VFS_NOT_VALID_PATH_ERROR) {
        printf("[vfs_chdir] Path %s not exsit!\n", pathname);
        return VFS_NOT_VALID_PATH_ERROR;
    }

    struct task_struct *current_task = current;
    current_task->cwd = target_file->dentry;

    return TRUE;
}

int vfs_mount(const char *device, const char *mountpoint, const char *filesystem)
{
    printf("================== vfs_mount ==================\n");
    struct vnode *target_file;
    char target_component_name[DNAME_INLINE_LEN]; // The component name of target file

    int loopUp_result = _lookUp_pathname(mountpoint, &target_file, target_component_name);
    if (loopUp_result == VFS_NOT_VALID_PATH_ERROR || loopUp_result == FALSE) {
        printf("[vfs_mount] Path %s not exsit!\n", mountpoint);
        return VFS_NOT_VALID_PATH_ERROR;
    }
    
    struct mount *mt = (struct mount *) kmalloc(sizeof(struct mount));
    
    mt->device_name = (char *) kmalloc(sizeof(char) * (strlen(device) + 1));
    strcpy(mt->device_name, device);

    if (!strcmp(filesystem, "tmpfs")) {
        printf("[vfs_mount] tmpfs filesystem mount\n");
        tmpfs.setup_mount(&tmpfs, mt, target_component_name);
        // We don't need to register tmpfs filesystem again, because tmpfs is kernel root file system
        //register_filesystem(&tmpfs);
    }

    if (!strcmp(filesystem, "fat32")) {
        printf("[vfs_mount] fat32 filesystem mount\n");
        register_filesystem(&fat32); 
        fat32.setup_mount(&fat32, mt, target_component_name); // TODO: Should modify target_component_name to "/" ?
    }

    target_file->dentry->mount = mt; // Say this directory is mount by other device/fs
    mt->root->parent = target_file->dentry; // Assign parent of this mountpoint as original dentry for go back to original fs 
    #ifdef __FS_DEBUG
    printf("mt->root->parent->name = %s\n", mt->root->parent->name);
    printf("mt->device_name = %s\n", mt->device_name);
    #endif

    return TRUE;
}

int vfs_unmount(const char* mountpoint)
{
    printf("================== vfs_unmount ==================\n");
    struct vnode *target_file;
    char target_component_name[DNAME_INLINE_LEN]; // The component name of target file

    int loopUp_result = _lookUp_pathname(mountpoint, &target_file, target_component_name);
    if (loopUp_result == VFS_NOT_VALID_PATH_ERROR || loopUp_result == FALSE) {
        printf("[vfs_mount] Path %s not exsit!\n", mountpoint);
        return VFS_NOT_VALID_PATH_ERROR;
    }

    if (target_file->dentry->parent->mount == NULL) {
        printf("target_file->dentry->parent->name = %s\n", target_file->dentry->parent->name);
        printf("The path %s not mounted before \n", mountpoint);
        return FALSE;
    }

    // For simplicity purposes, we not free the memory and just set direcotry to unmount status(assign NULL achive unmount)
    // TODO: For more robust funtionality, free all used memory
    target_file->dentry->parent->mount = NULL;
    return TRUE;
}

void vfs_print_directory_by_pathname(const char *pathname) 
{
    struct file *file = vfs_open(pathname, 0);
    struct dentry *dir_dentry = file->vnode->dentry;
    struct dentry *pos;
    list_for_each_entry(pos, &dir_dentry->sub_dirs, list) {
        printf("[vfs_ls] - %s\n", pos->name);
    }
}

void _vfs_dump_vnode(struct vnode *vnode)
{
    printf("=========Dump Vnode=========\n");

    switch (vnode->v_type) {
    case REGULAR_FILE:
        printf("vnode->type = REGULAR_FILE\n");
        break;
    case DIRECTORY:
        printf("vnode->type = DIRECTORY\n");
    default:
        printf("vnode->type = Unknown value %d\n", vnode->v_type);
    }
}

void _vfs_dump_dentry(struct dentry *dentry)
{
    printf("=========Dump Dentry=========\n");
    printf("Dentry name = %s\n", dentry->name);
    printf("=============================\n");
}

void _vfs_dump_file_struct()
{
    printf("=========Dump file struct (current task)=========\n");
    struct files_struct *m_files_struct = &current->files; // get current task

    printf("files_struct->count = %d\n", m_files_struct->count);
    printf("files_struct->next_fd = %d\n", m_files_struct->next_fd);
    printf("file descriptor table:\n");
    for (int i = 0;i < NR_OPEN_DEFAULT;i++) {
        if (m_files_struct->fd_array[i] == NULL)
            printf("fd_array[%d] = NULL \n", i);
        else
            printf("fd_array[%d] = 0x%x \n", i);
    }
    
    printf("=================================================\n");
}


void vfs_ls_print_test(const char *pathname)
{
    printf("\n--------------------> vfs ls <----------------------\n");
    printf("Requested pathname : %s", pathname);
    vfs_print_directory_by_pathname(pathname);
    printf("-------------------------------------------------------\n");
}

/** 
 * Test cases for Lab7
 */ 
void Lab7_fat32_test()
{
    printf("\n--------------> Lab7 fat32 test | Lab7_fat32_test() <--------------\n");
    //vfs_ls_print_test("/sdp1"); // TODO: Find out sdp1 dentry, but child directory not loaded from SDCARD
    // Rquirement1 - Get the FAT32 partition.
    _dump_fat32_metadata(&fat32_metadata);

    // Requirement 2 -  Look up and open a file in FAT32. Read / Write a file in FAT32.
    // Read
    char buf[BLOCK_SIZE];
    struct file *f1 = vfs_open("/sdp1/FATTEST.TXT", 0);
    int sz = vfs_read(f1, buf, 100);
    printf("[Lab7_fat32_test] Read size: %d, content: %s\n", sz, buf); // Should be "Fat32 Test file Content.""
    vfs_close(f1);
    // Write 
    f1 = vfs_open("/sdp1/FATTEST.TXT", 0);
    int sz2 = vfs_write(f1, "Hello World! This is Lab7 - FAT32 with SD card device", 53);
    printf("[Lab7_fat32_test] Write size: %d\n", sz2); // Should be 53.
    
    // Elective 1, Create a Fat32 file
    vfs_open("/sdp1/NEW.TXT", O_CREAT);

    vfs_ls_print_test("/sdp1");
}

/**
 * Test cases for Lab6 - VFS
 */
void vfs_test()
{
    printf("\n--------------> vfs simple teset | vfs_test() <--------------");
    struct file *a = vfs_open("/Test0", 0);
    if (a == NULL) printf("[vfs_test]pass1\n");
    a = vfs_open("/Test0", O_CREAT);
    if (a != NULL) printf("[vfs_test]pass2\n");
    vfs_close(a);
    struct file *b = vfs_open("/Test0", 0);
    if (b != NULL) printf("[vfs_test]pass3\n");
    vfs_close(b);
}

void vfs_requirement1_test()
{
    printf("\n--------------> vfs_requirement1_test() <--------------");
    /*　Requirement 1 */
    struct file *a = vfs_open("/hello", O_CREAT);
    struct file *b = vfs_open("/world", O_CREAT);
    vfs_write(a, "hello ", 6);
    vfs_write(b, "world!", 6);
    vfs_close(a);
    vfs_close(b);

    char buf[500];
    b = vfs_open("/hello", 0);
    a = vfs_open("/world", 0);
    int sz;
    sz = vfs_read(b, buf, 3);
    sz += vfs_read(b, buf + sz, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    printf("[vfs_requirement1_test] read size: %d, content: %s\n", sz, buf); // should be Hello World!
}

void vfs_requirement1_read_file_populated_in_cpio()
{
    printf("\n--------------> vfs_requirement1_read_file_populated_in_cpio() <--------------");
    const char *filename = "/initramfs/new.txt";
    struct file *a = vfs_open(filename, 0);
    char buf[500];
    int sz = vfs_read(a, buf, 100);
    printf("Content Size: %d, Content in %s file : \n", sz, filename);
    printf("%s\n", buf);

    vfs_ls_print_test("/initramfs/");
    printf("\n");
}

/* ls <directory> user program to list the target directory. */
void user_ls_process(const char *pathname)
{
    printf("=============== user_ls_process ===============");
    int fd = call_sys_open(pathname, 0);
    if (fd < 0) {
        return;
    }

    char *buf_dir = call_sys_read_directory(fd);
    int buf_idx = 0;
    // print direcotry
    printf("Requested pathname : %s\n", pathname);
    while (buf_dir[buf_idx] != (uint8_t)EOF) {
        printf("- %s\n", buf_dir + buf_idx);
        buf_idx += strlen(buf_dir + buf_idx) + 1;
    }
}

void vfs_user_process_test()
{
    printf("--------------> Lab6 req2 vfs user process syscall | vfs_user_process_test<--------------");
    // Lab6 - Requirement 2
    int a = call_sys_open("/user_hello", O_CREAT);
    int b = call_sys_open("/user_world", O_CREAT);
    call_sys_write(a, "Hello ", 6);
    call_sys_write(b, "World!", 6);
    call_sys_close(a);
    call_sys_close(b);

    char buf[500];
    b = call_sys_open("/user_hello", 0);
    a = call_sys_open("/user_world", 0);
    int sz;
    sz = call_sys_read(b, buf, 100);
    sz += call_sys_read(a, buf + sz, 100);
    buf[sz] = '\0';
    printf("[vfs_user_process_test] read size = %d, content = %s\n", sz, buf); // should be Hello World!

    // Elective 1, ls syscall
    printf("\n--------------------> Elevtive 1 - ls syscall <----------------------\n");
    user_ls_process("/");

    call_sys_exit();
}

void vfs_elective2_user_process_test()
{
    printf("--------------> Lab6 eletive2 | vfs_elective2_user_process_test()<--------------\n");
    char buf[8];
    call_sys_mkdir("mnt");
    int fd = call_sys_open("/mnt/a.txt", O_CREAT);
    call_sys_write(fd, "Hi", 2);
    call_sys_close(fd);
    call_sys_chdir("mnt");
    fd = call_sys_open("./a.txt", 0);
    if (fd < 0) {
        printf("Open ./a.txt Error!\n");
    }
    int sz = call_sys_read(fd, buf, 2);
    printf("size = %d, content = %s\n", sz, buf);

    call_sys_chdir("..");
    call_sys_mount("tmpfs", "mnt", "tmpfs");
    fd = call_sys_open("mnt/a.txt", 0);
    if (fd >= 0) {
        printf("Error, mnt/a.txt should not exist!");
    }

    call_sys_unmount("/mnt");
    fd = call_sys_open("/mnt/a.txt", 0);
    if (fd < 0) {
        printf("Open /mnt/a.txt Error!\n");
    }
    sz = call_sys_read(fd, buf, 2);
    printf("size = %d, content = %s\n", sz, buf);

    call_sys_exit();
    // call_sys_mkdir("newDir");
    // call_sys_open("newDir/newFile", O_CREAT);
    // call_sys_open("newDir/newFile87", O_CREAT);
    // call_sys_open("newDir/newFile999", O_CREAT);


    // vfs_ls_print_test("newDir");

    // struct task_struct *current_task = current;

    // printf("current_task->cwd = %s\n", current_task->cwd->name);


    // sys_chdir("newDir");
    // printf("current_task->cwd = %s\n", current_task->cwd->name);
    // user_ls_process(".");

    // sys_chdir("..");
    // printf("current_task->cwd = %s\n", current_task->cwd->name);
    // user_ls_process(".");

    // // test mount
    // call_sys_mkdir("mnt");
    // call_sys_mount("tmpfs", "/mnt", "tmpfs");
    // user_ls_process(".");

    // sys_chdir("mnt");

    // int fd = call_sys_open("/mnt/a.txt", O_CREAT);
    // call_sys_write(fd, "Hi", 2);
    // call_sys_close(fd);
    // fd = call_sys_open("./a.txt", 0);
    // char buf[100];
    // int sz = call_sys_read(fd, buf, 10);
    // printf("size = %d, content = %s\n\n", sz, buf);

    // user_ls_process(".");
    // fd = call_sys_open("/mnt/a.txt", 0);
    // if (fd < 0) {
    //     printf("Open file error\n");
    // }
    // call_sys_unmount("/mnt");
    // user_ls_process("/mnt");
    // fd = call_sys_open("/mnt/a.txt", 0);
    // if (fd < 0) {
    //     printf("Open file error\n");
    // }
   
}

