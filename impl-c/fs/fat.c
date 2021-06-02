#include "fs/fat.h"
#include "fs/vfs.h"

#include "dev/mbr.h"
#include "dev/sd.h"

#include "mm.h"
#include "stdint.h"
#include "string.h"
#include "uart.h"
#include <stddef.h>

#include "config.h"
#include "log.h"
#include "test.h"

#ifdef CFG_LOG_FAT
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

#define FAT_NODE_TYPE_DIR 1
#define FAT_NODE_TYPE_FILE 2
typedef struct {
  char *name;
  uint8_t node_type;
  size_t capacity;
  size_t size;
  uint32_t start_cluster_id; // Start idx inside the SD card
} Content;
#define content_ptr(vnode) (Content *)((vnode)->internal)

// Store the key inforamtion about the FAT File system in SD card
struct BackingStoreInfo {
  // Start lba of the FAT file system in the SD card
  uint32_t parition_start_lba;
  uint32_t cluster_begin_lba;
  uint32_t fat_begin_lba;
  uint32_t root_cluster;
  uint32_t sec_per_cluster;
};

// lazy init
int fat_initialized = false;
struct vnode_operations *fat_v_ops = NULL;
struct file_operations *fat_f_ops = NULL;

struct filesystem fat = {
    .name = "fat32", .setup_mount = fat_setup_mount, .next = NULL};

struct BackingStoreInfo fatConfig;

// Parse FAT layout information from the
//  first sector of the filesystem(VolumnID)
static int get_fat_config(uint32_t partition_lba_begin,
                          struct BackingStoreInfo *keyInfo);

static int parse_backing_store_info();

static struct vnode *create_vnode(const char *name, uint8_t node_type,
                                  uint32_t start_cluster_id);

static inline uint32_t cluster2lba(uint32_t clusterId) {
  return fatConfig.cluster_begin_lba +
         (clusterId - 2) * fatConfig.sec_per_cluster;
}

// Create & Initialize a empty vnode for FAT
// static struct vnode *create_vnode(const char *name, uint8_t node_type);

#define FATAL(msg)                                                             \
  uart_println(msg);                                                           \
  while (1) {                                                                  \
    ;                                                                          \
  }

void fat_dev() {
  fat_init();
  create_vnode("/", FAT_NODE_TYPE_DIR, fatConfig.root_cluster);
}

struct vnode *create_vnode(const char *name, uint8_t node_type,
                           uint32_t start_cluster_id) {
  struct vnode *node = (struct vnode *)kalloc(sizeof(struct vnode));

  // This node is not mounted by other directory
  node->mnt = NULL;
  node->v_ops = fat_v_ops;
  node->f_ops = fat_f_ops;

  Content *cnt = kalloc(sizeof(Content));
  {
    cnt->name = (char *)kalloc(sizeof(char) * strlen(name));
    strcpy(cnt->name, name);
    cnt->node_type = node_type;
    cnt->start_cluster_id = start_cluster_id;
  }
  node->internal = cnt;
  log_println("create node: `%s`, cluster_id: %d", cnt->name,
              cnt->start_cluster_id);
  return node;
}

int fat_write(struct file *f, const void *buf, unsigned long len) { return 0; }
int fat_read(struct file *f, void *buf, unsigned long len) { return 0; }
int fat_lookup(struct vnode *dir_node, struct vnode **target,
               const char *component_name) {
  return 0;
}
int fat_create(struct vnode *dir_node, struct vnode **target,
               const char *component_name) {
  return 0;
}

int fat_init() {
  fat_v_ops = kalloc(sizeof(struct vnode_operations));
  fat_v_ops->lookup = fat_lookup;
  fat_v_ops->create = fat_create;

  fat_f_ops = kalloc(sizeof(struct file_operations));
  fat_f_ops->read = fat_read;
  fat_f_ops->write = fat_write;

  if (0 != parse_backing_store_info()) {
    FATAL("Could not parse FAT information from SD card");
    return 1;
  }
  uart_println("fat init finished");
  fat_initialized = true;
  return 0;
}

int fat_setup_mount(struct filesystem *fs, struct mount *mount) {
  if (fat_initialized == false) {
    fat_init();
  }
  uart_println("fat setup");
  return 0;
}

int get_fat_config(uint32_t partition_lba_begin,
                   struct BackingStoreInfo *keyInfo) {
  if (keyInfo == NULL) {
    return -1;
  }

  int ret_val = 0;
  uint8_t *bfr = (uint8_t *)kalloc(512);
  readblock(partition_lba_begin, bfr);

  if (bfr[510] != 0x55 || bfr[511] != 0xAA) {
    ret_val = 1;
    goto _fat_vol_parse_end;
  }
  uintptr_t sec_start = (uintptr_t)bfr;

  uint16_t signature = *(uint32_t *)(sec_start + 0x1FE);
  if (signature != 0xAA55) {
    FATAL("Corrupted FAT VolumnID");
  }

  uint8_t sec_per_clus, num_alloc_tables;
  uint16_t byts_per_sec, rsvd_sec_cnt;
  uint32_t sec_per_alloc_table, root_cluster;

  byts_per_sec = *(uint16_t *)(sec_start + 0x0B); // value should always be 512
  sec_per_clus = *(uint8_t *)(sec_start + 0x0D);  // Sector per cluster
  rsvd_sec_cnt = *(uint16_t *)(sec_start + 0x0E);
  num_alloc_tables = *(uint8_t *)(sec_start + 0x10); // always = 2
  sec_per_alloc_table = *(uint32_t *)(sec_start + 0x24);
  root_cluster = *(uint32_t *)(sec_start + 0x2C); // Usually 0x00000002
  ret_val = 0;

  if (num_alloc_tables != 2 || byts_per_sec != 512) {
    uart_println("[FAT] numFAT:%d , bytePerSec:%d", num_alloc_tables,
                 byts_per_sec);
    FATAL("FAT format not supported");
  }

  keyInfo->parition_start_lba = partition_lba_begin;
  keyInfo->cluster_begin_lba = partition_lba_begin + rsvd_sec_cnt +
                               (num_alloc_tables * sec_per_alloc_table);
  keyInfo->fat_begin_lba = partition_lba_begin + rsvd_sec_cnt;
  keyInfo->root_cluster = root_cluster;
  keyInfo->sec_per_cluster = sec_per_clus;

_fat_vol_parse_end:
  kfree(bfr);
  return ret_val;
}

int parse_backing_store_info() {
  int ret = 0;

  unsigned char *buf = (unsigned char *)kalloc(512);
  // Read Partition info in SD card
  // The first block of the FAT filesystem is mbr
  readblock(0, buf);
  struct MBR *mbr = (struct MBR *)buf;
  int num_parti;

  if (!mbr_is_valid(mbr)) {
    FATAL("Read invalid MBR");
  } else if (0 >= (num_parti = mbr_num_partitions(mbr))) {
    FATAL("No valid partition exists");
  }

  // Assumming that the first partition it's a FAT file system
  uint32_t partition_start_lba = mbr->partition_entry[0].lba_begin;
  log_println("[FAT] FAT partition lba: %d", partition_start_lba);

  int fail;
  if (1 == (fail = get_fat_config(partition_start_lba, &fatConfig))) {
    uart_println("parse failed");
    ret = 1;
    goto _fat_parse_end;
  }

  log_println("[FAT] read config: fat lba: %d", fatConfig.fat_begin_lba);
  log_println("[FAT] read config: cluster lba: %d",
              fatConfig.cluster_begin_lba);
  log_println("[FAT] read config: root cluster: %d", fatConfig.root_cluster);

_fat_parse_end:
  kfree(buf);
  return ret;
}

#ifdef CFG_RUN_FS_FAT_TEST
#endif
void test_fat() {
#ifdef CFG_RUN_FS_FAT_TEST
#endif
}