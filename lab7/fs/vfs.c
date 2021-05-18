#include "vfs.h"
#include <types.h>
#include <printf.h>
#include <varied.h>
#include <string.h>
#include <test.h>

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
  int i;
  for(i = 0; i < REG_FS_NUM; i++) {
    if(registed_fs[i].name == null) {
      registed_fs[i] = *fs;
      return 0;
      break;
    }
  }
  return -1;
}

struct file* vfs_open(const char* pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  struct vnode* dir_node, *v_node;
  struct file* _file;
  char buff[FILE_NAME_LEN];
  int i, len;
  
  if(strlen(pathname) == 1 && pathname[0] == '/') {
    //root directory
    _file = (struct file* )varied_malloc(sizeof(struct file));

    if(_file == null)
      return null;

    // 2. Create a new file descriptor for this vnode if found.
    _file->vnode = rootfs->root;
    _file->f_pos = 0;
    _file->flags = flags;
    _file->f_ops = rootfs->root->f_ops;  
    printf("root directory\n");
    
    return _file;
  }
  i = 0;
  while(pathname[i] == '/')
    i++;

  len = 0;
  dir_node = rootfs->root;
  for(; i < strlen(pathname); i++) {
    
    if(pathname[i] == '/') {
      buff[len] = '\0';
     
      v_node = null;
      rootfs->root->v_ops->lookup(dir_node, &v_node, buff);
      
      if(v_node != null)
        dir_node = v_node;

      buff[0] = '\0';
      len = 0;
    }
    else {
      buff[len++] = pathname[i];
    }
  }

  buff[len] = '\0';
  v_node = null;
  rootfs->root->v_ops->lookup(dir_node, &v_node, buff);

  // 3. Create a new file if O_CREAT is specified in flags.
  if(v_node == null) { 
    if(flags & O_CREAT)
      rootfs->root->v_ops->create(dir_node, &v_node, buff);
    else
      return null;   
  }
  
  if(v_node == null)
    return null;
  
  _file = (struct file* )varied_malloc(sizeof(struct file));

  if(_file == null)
    return null;
  
  // 2. Create a new file descriptor for this vnode if found.
  _file->vnode = v_node;
  _file->f_pos = 0;
  _file->flags = flags;
  _file->f_ops = v_node->f_ops;  
  
  return _file;
}
int vfs_close(struct file* file) {
  // 1. release the file descriptor
  varied_free(file);
  file = null;
  return 0;
}
int vfs_write(struct file* file, const void* buf, size_t len) {
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
  return file->f_ops->write(file, buf, len);
}
int vfs_read(struct file* file, void* buf, size_t len) {
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
  return file->f_ops->read(file, buf, len);
}

void root_fs_init() {
  int err;
  //init tmpfs
  /*extern void* tmpfs_init();
  tmpfs_init();
  
  rootfs = (struct mount*)varied_malloc(sizeof(struct mount));

  if(rootfs == null)
    return;
  
  err = registed_fs[0].setup_mount(&registed_fs[0], rootfs);
  
  if(err == -1) {
    printf("mount root fs error\n");
    return;
  }
  extern void tmpfs_load_initramfs(struct mount* mount);
  tmpfs_load_initramfs(rootfs); */
  
  //init fat32
  extern void* fat32_init();
  fat32_init();
  
  rootfs = (struct mount*)varied_malloc(sizeof(struct mount));

  if(rootfs == null)
    return;

  err = registed_fs[0].setup_mount(&registed_fs[0], rootfs);
  
  if(err == -1) {
    printf("mount root fs error\n");
    return;
  }
}

