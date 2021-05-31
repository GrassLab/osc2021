#include "fs/vfs.h"

#include "mm.h"
#include "string.h"
#include "uart.h"

#include "config.h"
#include "log.h"
#include "test.h"

#define PATH_DELIM '/'

#ifdef CFG_LOG_VFS
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

#define ENSURE_COND(cond, fail)                                                \
  if (!(cond)) {                                                               \
    goto fail;                                                                 \
  }

struct mount *rootfs;

#define __BUSY_WAIT                                                            \
  {                                                                            \
    while (1) {                                                                \
      ;                                                                        \
    }                                                                          \
  }

// filesystems registered
struct filesystem registered = {
    .name = "reserved-not-used",
    .setup_mount = NULL,
    .next = NULL,
};

/**
 * @brief Find a vnode for opening files
 * @param pathname full path of the target
 * @param flags FILE operation flags
 * @param root root vnode of the system
 */
static struct vnode *find_vnode(const char *pathname, int flags,
                                struct vnode *root);

void vfs_init() {
  rootfs = kalloc(sizeof(struct mount));
  if (rootfs == NULL) {
    uart_println("Cannot allocate space for rootfs ");
    __BUSY_WAIT
  }
  rootfs->root = NULL;
  rootfs->fs = NULL;
}

int mount_root_fs(const char *fs_impl) {
  struct filesystem *cur;
  struct filesystem *rootfs_target;
  for (cur = registered.next; cur != NULL; cur = cur->next) {
    if (strcmp(cur->name, fs_impl) == 0) {
      rootfs_target = cur;
    }
  }
  if (rootfs_target == NULL) {
    uart_println("Rootfs mount - Cannot found fs: %s", fs_impl);
    __BUSY_WAIT
  }
  log_println("rootfs impl found: %s", rootfs_target->name);
  rootfs_target->setup_mount(rootfs_target, rootfs);
  log_println("[vfs] rootfs build successfully");
  return 0;
}

int register_filesystem(struct filesystem *fs) {
  struct filesystem *cur;
  for (cur = registered.next; cur != NULL; cur = cur->next) {
    if (strcmp(cur->name, fs->name) == 0) {
      return 1;
    }
  }

  // Register the file system into kernel.
  struct filesystem *_first = registered.next;
  registered.next = fs;
  fs->next = _first;

#ifdef CFG_LOG_VFS
  log_println("Filesystems registered:");
  int i = 0;
  for (cur = registered.next, i = 0; cur != NULL; cur = cur->next, i++) {
    log_println(" [%d]: %s", i, cur->name);
  }
#endif
  return 0;
}

struct vnode *find_vnode(const char *pathname, int flags, struct vnode *root) {
  struct vnode *cwd = root;

  const char *path;
  char *query_name;
  int ret, start_idx, end_idx, name_size;
  struct vnode *target_child;

  path = pathname;
  query_name = NULL;
  ret = start_idx = end_idx = -1;
  // Resolve the componenet name until reach the end
  for (; 0 == (ret = get_component(path, &start_idx, &end_idx));
       query_name = NULL, start_idx = end_idx = -1) {

    // Copy name
    {
      name_size = end_idx - start_idx + 1;
      query_name = kalloc(name_size + 1);
      memcpy(query_name, &path[start_idx], name_size);
      query_name[name_size] = '\0';
    }

    // Query file by it's name
    cwd->v_ops->lookup(cwd, &target_child, query_name);

    // Child found, go to the next level
    if (target_child != NULL) {
      path = &path[end_idx + 1];
      cwd = target_child;
      kfree(query_name);
      continue;
    }

    // Child not found, handle properly and return...
    // Get next component to makesure this is the final level
    path = &path[end_idx + 1];
    ret = get_component(path, &start_idx, &end_idx);
    if (ret == 0) {
      // There're still component names not resolved, this is not the final
      // level
      uart_println("Not existing middle pathname: `%s`", query_name);
      kfree(query_name);
      return NULL;
    }
    if (flags & FILE_O_CREAT) {
      ret = cwd->v_ops->create(cwd, &target_child, query_name);
      kfree(query_name);
      return (ret == 0) ? target_child : NULL;
    } else {
      kfree(query_name);
      return NULL;
    }
  }
  return target_child;
}

struct file *vfs_open(const char *pathname, int flags) {

  struct vnode *target;
  target = find_vnode(pathname, flags, rootfs->root);
  if (target == NULL) {
    uart_println("[vfs] not found");
    return NULL;
  }

  log_println("[vfs] create file handle for `%s`", pathname);
  struct file *f = kalloc(sizeof(struct file));
  f->node = target;
  f->f_pos = 0;
  f->f_ops = target->f_ops;
  f->flags = 0; // TBD
  return f;
}

int vfs_close(struct file *file) {
  kfree(file);
  return 0;
}

int vfs_write(struct file *file, const void *buf, size_t len) {
  return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file *file, void *buf, size_t len) {
  return file->f_ops->read(file, buf, len);
  return 0;
}

int get_component(const char *path, /* Return*/ int *start_idx,
                  /* Return*/ int *end_idx) {
  if (start_idx == NULL || end_idx == NULL) {
    return -1;
  }
  if (path == NULL) {
    goto get_cmpt_failed;
  }

  *start_idx = -1;
  *end_idx = -1;
  const char *start;
  start = ignore_leading(path, PATH_DELIM);
  if (start == NULL) {
    goto get_cmpt_failed;
  }
  *start_idx = start - path;

  const char *next_delim;
  next_delim = strchr(start, PATH_DELIM);
  if (next_delim != NULL) {
    *end_idx = next_delim - path - 1;
  } else {
    // hit the end of string
    *end_idx = strlen(start) + *start_idx - 1;
  }
  // Status check
  if (*start_idx != -1 && *end_idx != -1) {
    return 0;
  }

get_cmpt_failed:
  *start_idx = -1;
  *end_idx = -1;
  return -1;
}

#ifdef CFG_RUN_FS_VFS_TEST

// input_data, expect answer
#define RUN_COMP_TEST(path, ret, start, end)                                   \
  {                                                                            \
    char *_path = path;                                                        \
    int _ret = -2, _start = -2, _end = -2;                                     \
    _ret = get_component(_path, &_start, &_end);                               \
    if ((ret != _ret) || (_start != start) || (_end != end)) {                 \
      uart_println("<got>(<excepted>)");                                       \
      uart_println("path:%s, ret:%d(%d), start:%d(%d), end:%d(%d)", _path,     \
                   _ret, ret, _start, start, _end, end);                       \
      assert((ret == _ret) && (start == _start) && (end == _end));             \
    }                                                                          \
  }

bool test_get_component_good_input() {
  RUN_COMP_TEST("dev/a/b", 0, 0, 2);
  RUN_COMP_TEST("dev", 0, 0, 2);
  RUN_COMP_TEST("//dev/", 0, 2, 4);
  RUN_COMP_TEST("//dev", 0, 2, 4);
  RUN_COMP_TEST("//dev////", 0, 2, 4);
  RUN_COMP_TEST("////dev/", 0, 4, 6);
  RUN_COMP_TEST("/dev/", 0, 1, 3);
  RUN_COMP_TEST("/dev", 0, 1, 3);
  RUN_COMP_TEST("/dev////", 0, 1, 3);
  return true;
}

bool test_get_component_bad_input() {
  RUN_COMP_TEST("///", -1, -1, -1);
  RUN_COMP_TEST("/", -1, -1, -1);
  RUN_COMP_TEST("", -1, -1, -1);
  RUN_COMP_TEST(NULL, -1, -1, -1);
  return true;
}

#endif

void test_vfs() {
#ifdef CFG_RUN_FS_VFS_TEST
  unittest(test_get_component_good_input, "FS",
           "VFS - get component (good input)");
  unittest(test_get_component_bad_input, "FS",
           "VFS - get component (bad input)");
#endif
}