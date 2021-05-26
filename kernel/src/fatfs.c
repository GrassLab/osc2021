#include "fatfs.h"

#include "alloc.h"
#include "printf.h"
#include "sdhost.h"
#include "string.h"
#include "vfs.h"

int get_starting_sector(int cluster) {
  return (cluster - 2) * fat_boot_sector->sectors_per_cluster +
         data_starting_sector;
}

void fatfs_init() {
  fatfs_v_ops =
      (struct vnode_operations*)malloc(sizeof(struct vnode_operations));
  fatfs_v_ops->lookup = fatfs_lookup;
  fatfs_v_ops->set_parent = fatfs_set_parent;
  fatfs_f_ops = (struct file_operations*)malloc(sizeof(struct file_operations));
  fatfs_f_ops->write = fatfs_write;
  fatfs_f_ops->read = fatfs_read;
  fatfs_f_ops->list = fatfs_list;
  sd_init();
}

void fatfs_set_directory(struct fatfs_fentry* fentry,
                         struct fatfs_dentry* dentry) {
  for (int i = 0; i < MAX_FILES_IN_DIR; ++i) {
    int flag = 0;
    for (int j = 0; j < 8; j++) {
      // printf("0x%x ", (dentry + i)->filename[j]);
      // handle weird file
      if ((dentry + i)->filename[j] == 0) {
        flag = 1;
      }
    }
    // for (int j = 0; j < 3; j++) {
    //   printf("0x%x ", (dentry + i)->extension[j]);
    // }
    // printf("\n");
    if ((dentry + i)->filename[0] && !flag) {
      // printf("%d\n", (dentry + i)->file_size);
      strncpy(fentry->child[i]->name, (dentry + i)->filename, 8);
      size_t len = strlen(fentry->child[i]->name);
      fentry->child[i]->name_len = len;
      if ((dentry + i)->extension[0]) {
        *(fentry->child[i]->name + len) = '.';
        strncpy(fentry->child[i]->name + len + 1, (dentry + i)->extension, 3);
      }

      struct vnode* vnode = (struct vnode*)malloc(sizeof(struct vnode));
      vnode->mount = 0;
      vnode->v_ops = fentry->vnode->v_ops;
      vnode->f_ops = fentry->vnode->f_ops;
      vnode->internal = fentry->child[i];
      int starting_cluster =
          ((dentry + i)->cluster_high << 2) + (dentry + i)->cluster_low;
      int buf_size = (dentry + i)->file_size;
      fatfs_set_fentry(fentry->child[i], FILE_REGULAR, vnode, starting_cluster,
                       buf_size);
      // printf("%s\n", fentry->child[i]->name);
    }
  }
}

void fatfs_set_fentry(struct fatfs_fentry* fentry, FILE_TYPE type,
                      struct vnode* vnode, int starting_cluster, int buf_size) {
  fentry->starting_cluster = starting_cluster;
  fentry->vnode = vnode;
  fentry->type = type;
  fentry->buf = (struct fatfs_buf*)malloc(sizeof(struct fatfs_buf));
  fentry->buf->size = buf_size;
  for (int i = 0; i < FATFS_BUF_SIZE; i++) {
    fentry->buf->buffer[i] = '\0';
  }

  if (fentry->type == FILE_DIRECTORY) {
    for (int i = 0; i < MAX_FILES_IN_DIR; ++i) {
      fentry->child[i] =
          (struct fatfs_fentry*)malloc(sizeof(struct fatfs_fentry));
      fentry->child[i]->name[0] = 0;
      fentry->child[i]->type = FILE_NONE;
      fentry->child[i]->parent_vnode = vnode;
    }
  }
}

int fatfs_setup_mount(struct filesystem* fs, struct mount* mount) {
  char* mbr = (char*)malloc(BLOCK_SIZE);
  readblock(0, mbr);
  if (mbr[510] != 0x55 || mbr[511] != 0xAA) {
    printf("[fatfs_init] bad magic in MBR\n");
    return 0;
  }

  int entry_size = sizeof(struct mbr_partition_entry);
  struct mbr_partition_entry* entry =
      (struct mbr_partition_entry*)malloc(entry_size);
  char* src = (char*)mbr;
  char* dst = (char*)entry;
  for (int i = 0; i < entry_size; i++) {
    // printf("i: %d, 0x%x\n", i, src[MBR_PARTITION_BASE + i]);
    dst[i] = src[MBR_PARTITION_BASE + i];
  }
  free(mbr);

  printf("\n========== FAT32 init ==========\n");
  printf("Partition type: 0x%x", entry->partition_type);
  if (entry->partition_type == 0xB) {
    printf(" (FAT32 with CHS addressing)");
  }
  printf("\nPartition size: %d (sectors)\n", entry->sector_count);
  printf("Block index: %d\n", entry->starting_sector);
  printf("================================\n\n");
  fat_starting_sector = entry->starting_sector;

  char* fat_boot = (char*)malloc(BLOCK_SIZE);
  readblock(fat_starting_sector, fat_boot);
  int boot_sector_size = sizeof(struct fatfs_boot_sector);
  fat_boot_sector = (struct fatfs_boot_sector*)malloc(boot_sector_size);
  src = (char*)fat_boot;
  dst = (char*)fat_boot_sector;
  for (int i = 0; i < boot_sector_size; i++) {
    dst[i] = src[i];
  }
  free(fat_boot);

  int root_dir_sectors = 0;  // no root directory sector in FAT32
  data_starting_sector =
      fat_starting_sector + fat_boot_sector->reserved_sector_count +
      fat_boot_sector->fat_count * fat_boot_sector->sectors_per_fat_32 +
      root_dir_sectors;
  root_starting_sector = get_starting_sector(fat_boot_sector->root_cluster);
  // printf("%d\n", data_starting_sector);
  // printf("%d\n", root_starting_sector);

  char* fat_root = (char*)malloc(BLOCK_SIZE);
  readblock(root_starting_sector, fat_root);
  fat_root_dentry = (struct fatfs_dentry*)fat_root;

  struct fatfs_fentry* root_fentry =
      (struct fatfs_fentry*)malloc(sizeof(struct fatfs_fentry));
  struct vnode* vnode = (struct vnode*)malloc(sizeof(struct vnode));
  vnode->mount = mount;
  vnode->v_ops = fatfs_v_ops;
  vnode->f_ops = fatfs_f_ops;
  vnode->internal = (void*)root_fentry;
  root_fentry->parent_vnode = 0;
  fatfs_set_fentry(root_fentry, FILE_DIRECTORY, vnode,
                   fat_boot_sector->root_cluster, 4096);
  fatfs_set_directory(root_fentry, fat_root_dentry);
  mount->fs = fs;
  mount->root = vnode;
  return 1;
}

int fatfs_lookup(struct vnode* dir_node, struct vnode** target,
                 const char* component_name) {
  // printf("[lookup] %s\n", component_name);
  struct fatfs_fentry* fentry = (struct fatfs_fentry*)dir_node->internal;
  if (fentry->type != FILE_DIRECTORY) return 0;

  if (!strcmp(component_name, ".")) {
    *target = fentry->vnode;
    return 1;
  }
  if (!strcmp(component_name, "..")) {
    if (!fentry->parent_vnode) return 0;
    *target = fentry->parent_vnode;
    return 1;
  }

  for (int i = 0; i < MAX_FILES_IN_DIR; i++) {
    fentry = ((struct fatfs_fentry*)dir_node->internal)->child[i];
    if (!strcmp(fentry->name, component_name)) {
      *target = fentry->vnode;
      return 1;
    }
  }
  return 0;
}

int fatfs_set_parent(struct vnode* child_node, struct vnode* parent_vnode) {
  struct fatfs_fentry* fentry = (struct fatfs_fentry*)child_node->internal;
  fentry->parent_vnode = parent_vnode;
  return 1;
}

int fatfs_write(struct file* file, const void* buf, size_t len) {
  struct fatfs_fentry* fentry = (struct fatfs_fentry*)file->vnode->internal;
  for (size_t i = 0; i < len; i++) {
    fentry->buf->buffer[file->f_pos++] = ((char*)buf)[i];
    if (fentry->buf->size < file->f_pos) {
      fentry->buf->size = file->f_pos;
    }
  }

  for (int i = 0; i < MAX_FILES_IN_DIR; i++) {
    if (!strncmp((fat_root_dentry + i)->filename, fentry->name,
                 fentry->name_len)) {
      (fat_root_dentry + i)->file_size = fentry->buf->size;
      // printf("new file size: %d\n", (fat_root_dentry + i)->file_size);
    }
  }
  writeblock(root_starting_sector, (char*)fat_root_dentry);
  // printf("%s\n", fentry->buf->buffer);

  int starting_sector = get_starting_sector(fentry->starting_cluster);
  writeblock(starting_sector, fentry->buf->buffer);

  return len;
}

int fatfs_read(struct file* file, void* buf, size_t len) {
  size_t read_len = 0;
  struct fatfs_fentry* fentry = (struct fatfs_fentry*)file->vnode->internal;
  int starting_sector = get_starting_sector(fentry->starting_cluster);
  // printf("%d\n", starting_sector);
  readblock(starting_sector, fentry->buf->buffer);

  for (size_t i = 0; i < len; i++) {
    ((char*)buf)[i] = fentry->buf->buffer[file->f_pos++];
    read_len++;
    if (read_len == fentry->buf->size) {
      break;
    }
  }
  return read_len;
}

int fatfs_list(struct file* file, void* buf, int index) {
  struct fatfs_fentry* fentry = (struct fatfs_fentry*)file->vnode->internal;
  if (fentry->type != FILE_DIRECTORY) return -1;
  if (index >= MAX_FILES_IN_DIR) return -1;

  if (fentry->child[index]->type == FILE_NONE) return 0;
  strcpy((char*)buf, fentry->child[index]->name);
  return fentry->child[index]->buf->size;
}
