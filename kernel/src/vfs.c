#include "vfs.h"

#include "alloc.h"
#include "cpio.h"
#include "printf.h"
#include "string.h"
#include "thread.h"
#include "tmpfs.h"

void vfs_test() {
  const char* argv[] = {"vfs_test", 0};
  exec("vfs_test", argv);
}

void vfs_ls_test() {
  const char* argv[] = {"ls", ".", 0};
  exec("ls", argv);
}

void vfs_hard_test() {
  const char* argv[] = {"vfs_test_hard", 0};
  exec("vfs_test_hard", argv);
}

void vfs_init() {
  fs_list.head = 0;
  fs_list.tail = 0;
  // init and register tmpfs
  tmpfs_init();
  struct filesystem* tmpfs =
      (struct filesystem*)malloc(sizeof(struct filesystem));
  tmpfs->name = "tmpfs";
  tmpfs->setup_mount = tmpfs_setup_mount;
  register_filesystem(tmpfs);
  // use tmpfs to mount root filesystem
  rootfs = (struct mount*)malloc(sizeof(struct mount));
  struct filesystem* fs = get_fs_by_name("tmpfs");
  if (fs == 0) {
    printf("Mount root filesystem failed!!\n");
    return;
  }
  fs->setup_mount(fs, rootfs);
  current_dir = rootfs->root;
  cpio_populate_rootfs();
}

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
  if (fs_list.head == 0) {
    fs_list.head = fs;
    fs_list.head->next = 0;
    fs_list.tail = fs_list.head;
  } else {
    fs_list.tail->next = fs;
    fs_list.tail = fs_list.tail->next;
  }
  return 1;
}

struct filesystem* get_fs_by_name(const char* name) {
  for (struct filesystem* fs = fs_list.head; fs != 0; fs = fs->next) {
    if (!strcmp(fs->name, name)) {
      return fs;
    }
  }
  printf("Filesystem \'%s\' not found!!\n", name);
  return 0;
}

int vfs_find_vnode(struct vnode** target, const char* pathname) {
  if (!strcmp(pathname, "/")) {
    *target = rootfs->root;
    return 1;
  }

  char* pathname_ = (char*)malloc(strlen(pathname) + 1);
  strcpy(pathname_, pathname);
  struct vnode* dir = current_dir;
  if (pathname_[0] == '/') dir = rootfs->root;
  printf("find node: %s\n", pathname_);

  char* component_name = strtok(pathname_, '/');
  while (component_name && *component_name != '\0') {
    int found = dir->v_ops->lookup(dir, target, component_name);
    printf("component name: %s, found %d\n", component_name, found);
    if (!found) {
      printf("No such file or directory: %s\n", pathname);
      free(pathname_);
      return 0;
    }
    dir = *target;
    component_name = strtok(0, '/');
  }
  free(pathname_);
  return 1;
}

struct file* vfs_open(const char* pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
  struct vnode* dir = current_dir;
  struct vnode* target = 0;
  struct file* fd = 0;

  char* pathname_ = (char*)malloc(strlen(pathname) + 1);
  strcpy(pathname_, pathname);
  // pathname: /mnt/file1 -> pathname_: /mnt,  filename: file1
  // pathname: file1      -> pathname_: file1, filename: NULL
  char* filename = split_last(pathname_, '/');

  if (filename == 0) {
    filename = pathname_;
  } else {
    int prefix_found = vfs_find_vnode(&dir, pathname_);
    if (!prefix_found) {  // e.g., given pathname /abc/zxc/file1, but /abc/zxc
                          // not found
      printf("Path does not exist: %s\n", pathname);
      free(pathname_);
      return 0;
    }
  }
  free(pathname_);

  int file_found = dir->v_ops->lookup(dir, &target, filename);
  if (flags == O_CREAT) {
    if (!file_found) {
      dir->v_ops->create(dir, &target, filename, FILE_REGULAR);
      fd = (struct file*)malloc(sizeof(struct file));
      fd->vnode = target;
      fd->f_ops = target->f_ops;
      fd->f_pos = 0;
    } else {
      printf("File already exists: %s\n", pathname);
    }
  } else {
    if (!file_found) {
      printf("File does not exist: %s\n", pathname);
    } else {
      fd = (struct file*)malloc(sizeof(struct file));
      fd->vnode = target;
      fd->f_ops = target->f_ops;
      fd->f_pos = 0;
    }
  }
  return fd;
}

int vfs_close(struct file* file) {
  // 1. release the file descriptor
  free(file);
  return 1;
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

int vfs_list(struct file* file, void* buf, int index) {
  return file->f_ops->list(file, buf, index);
}

int vfs_mkdir(const char* pathname) {
  printf("%s\n", pathname);
  struct vnode* dir = current_dir;
  struct vnode* target = 0;

  char* pathname_ = (char*)malloc(strlen(pathname) + 1);
  strcpy(pathname_, pathname);
  // pathname: /mnt/file1 -> pathname_: /mnt,  filename: file1
  // pathname: file1      -> pathname_: file1, filename: NULL
  char* dirname = split_last(pathname_, '/');

  if (dirname == 0) {
    dirname = pathname_;
  } else {
    int prefix_found = vfs_find_vnode(&dir, pathname_);
    if (!prefix_found) {  // e.g., given pathname /abc/zxc/file1, but /abc/zxc
                          // not found
      printf("Path does not exist: %s\n", pathname);
      free(pathname_);
      return 0;
    }
  }
  free(pathname_);

  int file_found = dir->v_ops->lookup(dir, &target, dirname);
  if (file_found) {
    printf("File already exists: %s\n", pathname);
    return 0;
  }
  return dir->v_ops->create(dir, &target, dirname, FILE_DIRECTORY);
}

int vfs_chdir(const char* pathname) {
  struct vnode* target = 0;
  int dir_found = vfs_find_vnode(&target, pathname);
  if (!dir_found) {
    printf("Directory does not exist: %s\n", pathname);
    return 0;
  }
  current_dir = target;
  return 1;
}
