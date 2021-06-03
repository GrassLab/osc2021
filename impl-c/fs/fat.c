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

  void *data;                // data in memory (directly copied from SDcard)
  uint32_t start_cluster_id; // Start idx inside the SD card

  // Cached information
  bool cached;
  // Only used in dir node
  struct vnode **children;
} Content;
#define content_ptr(vnode) (Content *)((vnode)->internal)

static const char *node_name(struct vnode *node) {
  Content *cnt = content_ptr(node);
  return cnt->name;
}

// Store the key inforamtion about the FAT File system in SD card
struct BackingStoreInfo {
  // Start lba of the FAT file system in the SD card
  uint32_t parition_start_lba;
  uint32_t cluster_begin_lba;
  uint32_t fat_begin_lba;
  uint32_t root_cluster;
  uint32_t sec_per_cluster;
  uint32_t sec_per_alloc_table;

  // FAT table
  uint32_t *alloc_table; // Array of 32 bit integers
  uint32_t alloc_table_entry_cnt;
};

// Directory entry in the FAT file systems
struct Dentry {
  char name[8];
  char extension[3];
  union {
    uint8_t val;
    struct __attribute__((packed)) _Dentry_attr {
      uint8_t read_only : 1;
      uint8_t hidden : 1;
      uint8_t system : 1;
      uint8_t volume_id : 1;
      uint8_t dir : 1;
      uint8_t archive : 1;
      uint8_t _unused : 2;
    } fields;

  } attr;                // 1 Bytes
  uint8_t _reserved1[8]; // reserved
  uint16_t first_cluster_high;
  uint8_t _reserved2[4]; // reserved
  uint16_t first_cluster_low;
  uint32_t size;
} __attribute__((packed));

enum FAT_DentryType {
  FAT_DENTRY_END = 0x0,
  FAT_DENTRY_UNUSED = 0xE5,

  // Short/Long file name
  FAT_DENTRY_SFN = 0x1,
  FAT_DENTRY_LFN = 0x2,
};

static enum FAT_DentryType dentry_type(struct Dentry *dentry);

// lazy init
int fat_initialized = false;
struct vnode_operations *fat_v_ops = NULL;
struct file_operations *fat_f_ops = NULL;

struct filesystem fat = {
    .name = "fat32", .setup_mount = fat_setup_mount, .next = NULL};

struct BackingStoreInfo fatConfig;

#define FATAL(msg)                                                             \
  uart_println(msg);                                                           \
  while (1) {                                                                  \
    ;                                                                          \
  }

static int parse_backing_store_info();
static int get_fat_config(uint32_t partition_lba_begin,
                          struct BackingStoreInfo *keyInfo);
static uint32_t fetch_file(uint32_t begin_lba, void **ret_file);
static int follow_cluster_chain(uint32_t cluster_idx,
                                /* Return */ size_t *ret_len);
static struct vnode *create_vnode(const char *name, uint8_t node_type,
                                  uint32_t start_cluster_id);
static inline uint32_t cluster2lba(uint32_t clusterId) {
  return fatConfig.cluster_begin_lba +
         (clusterId - 2) * fatConfig.sec_per_cluster;
}
static int build_dir_cache(struct vnode *dir_node);
static struct vnode *create_vnode_from_dentry(struct Dentry *dentry);

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
    cnt->size = 0;
    cnt->capacity = 0;
    cnt->start_cluster_id = start_cluster_id;
    cnt->data = NULL;
    cnt->cached = false;
    cnt->children = NULL;
  }
  node->internal = cnt;
  log_println("create vnode: `%s`, cluster_id: %d", cnt->name,
              cnt->start_cluster_id);
  return node;
}

void fat_dev() {
  fat_init();
  void *root_data;
  uint32_t num_sector;
  num_sector = fetch_file(fatConfig.root_cluster, &root_data);

  struct vnode *root_dir =
      create_vnode("/", FAT_NODE_TYPE_DIR, fatConfig.root_cluster);
  Content *root_content = content_ptr(root_dir);
  // populate root dir content
  root_content->data = root_data;
  // Number of dentries (not all are valid) described in the backing store
  root_content->capacity = num_sector * (512 / sizeof(uint32_t));

  struct vnode *target;
  int ret;
  ret = fat_lookup(root_dir, &target, "FIXUP.DAT");
  if (ret == 0) {
    uart_println("found node: %s", node_name(target));
  } else {
    uart_println("failed to find node");
  }
}

int fat_setup_mount(struct filesystem *fs, struct mount *mount) {
  if (fat_initialized == false) {
    fat_init();
    fat_initialized = true;
  }

  // TODO: setup root node

  uart_println("fat setup");
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

  // Copy the allocation table to memory
  fatConfig.alloc_table =
      kalloc(fatConfig.alloc_table_entry_cnt * sizeof(uint32_t));
  uintptr_t addr_table = (uintptr_t)fatConfig.alloc_table;
  for (size_t i = 0; i < fatConfig.sec_per_alloc_table; i++) {
    readblock(fatConfig.fat_begin_lba + i, (void *)(addr_table + i * 512));
  }

  uart_println("fat init finished");
  return 0;
}

int fat_write(struct file *f, const void *buf, unsigned long len) { return 0; }

int fat_read(struct file *f, void *buf, unsigned long len) { return 0; }

int fat_lookup(struct vnode *dir_node, struct vnode **target,
               const char *component_name) {

  Content *dir_content = content_ptr(dir_node);
  if (dir_content->node_type != FAT_NODE_TYPE_DIR) {
    uart_println("Only able to lookup dir");
    return -1;
  }
  log_println("Lookup `%s` under node:`%s`", component_name,
              node_name(dir_node));
  // we only have it's vnode but not data
  if (dir_content->data == NULL) {
    uart_println("TODO: fetch data from backing store");
    while (1) {
      ;
    }
  }

  if (dir_content->cached == false) {
    if (0 != build_dir_cache(dir_node)) {
      FATAL("Could not build cache for dentry");
    }
  }

  dir_content = content_ptr(dir_node);
  const char *child_name;
  for (int i = 0; i < dir_content->size; i++) {
    child_name = node_name(dir_content->children[i]);
    if (0 == strcmp(child_name, component_name)) {
      if (target != NULL) {
        *target = dir_content->children[i];
      }
      return 0;
    }
  }
  return -1;
}

int fat_create(struct vnode *dir_node, struct vnode **target,
               const char *component_name) {
  return 0;
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

/**
 * @brief Parse FAT layout information from the first sector of the
 * filesystem(VolumnID)
 */
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
    goto _get_fat_config_end;
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
  keyInfo->sec_per_alloc_table = sec_per_alloc_table;

  // The Allocation table contains entries with 32 Byte each.
  // Total number of entries in the allocation table could be derived from
  //  num_entries =  entries_per_sec * sectors_per_table
  // Knowing that each sector contains 512 Bytes, we get `entries_per_sec` = 16
  uint32_t num_entries = (byts_per_sec / 32) * sec_per_alloc_table;
  keyInfo->alloc_table_entry_cnt = num_entries;
  log_println("[FAT] entries in FAT: %d", num_entries);

_get_fat_config_end:
  kfree(bfr);
  return ret_val;
}

/**
 * @brief Get the length of the file by following links in the allocation table
 * @param ret_len length of the file
 * @retval 0 for success, fail otherwise
 */
int follow_cluster_chain(uint32_t cluster_idx, /* Return */ size_t *ret_len) {
  if (cluster_idx < 0 || ret_len == NULL) {
    return -1;
  }
  size_t len = 1;
  uint32_t next;
  for (uint32_t idx = cluster_idx;; idx = next, len++) {
    next = fatConfig.alloc_table[idx];
    if (next >= 0xFFFFFF8) {
      break;
    }
  }
  *ret_len = len;
  return 0;
}

/**
 * @brief Fetch the entire file from SD card to memory
 * @warning would allocate a new memory space
 * @param ret_file pointer to the file
 * @retval len of the file
 **/
uint32_t fetch_file(uint32_t cluster_idx, void **ret_file) {
  if (ret_file == NULL) {
    return -1;
  }
  size_t num_sectors;
  if (0 != follow_cluster_chain(cluster_idx, &num_sectors)) {
    *ret_file = NULL;
    return -1;
  }
  // Copy the entire file into a contiguous memory space
  uintptr_t buffer = (uintptr_t)kalloc(num_sectors * 512);
  uint32_t cur_idx = cluster_idx;
  for (size_t i = 0; i < num_sectors; i++) {
    readblock(cluster2lba(cur_idx), (void *)(buffer + i * 512));
    cur_idx = fatConfig.alloc_table[cur_idx];
  }
  *ret_file = (void *)buffer;
  return num_sectors;
}

static enum FAT_DentryType dentry_type(struct Dentry *dentry) {
  uint8_t *first_byte = (uint8_t *)dentry;
  switch (*first_byte) {
  case FAT_DENTRY_UNUSED:
    return FAT_DENTRY_UNUSED;
  case FAT_DENTRY_END:
    return FAT_DENTRY_END;
  }
  // All lower 4 bits are set
  if ((dentry->attr.val & 0xF) == 0xF) {
    return FAT_DENTRY_LFN;
  }
  return FAT_DENTRY_SFN;
}

struct vnode *create_vnode_from_dentry(struct Dentry *dentry) {
  // SFN
  char name[13]; // 8(name) + 1('.') + 3(extension) + 1('\')
  {
    int size = 0;
    for (int i = 0; i < 8; i++) {
      if (dentry->name[i] == ' ')
        break;
      name[size++] = dentry->name[i];
    }
    int put_delim = 1;
    for (int i = 0; i < 3; i++) {
      if (dentry->extension[i] == ' ')
        break;
      if (put_delim == 1) {
        name[size++] = '.';
        put_delim = 0;
      }
      name[size++] = dentry->extension[i];
    }
    name[size++] = '\0';
  }

  int vnode_type =
      dentry->attr.fields.dir == 1 ? FAT_NODE_TYPE_DIR : FAT_NODE_TYPE_FILE;

  // TODO: upper four bit should be cleared. since it's reserved
  uint32_t cluster_id =
      (dentry->first_cluster_high << 16) + dentry->first_cluster_low;

  struct vnode *node = create_vnode(name, vnode_type, cluster_id);
  return node;
}

int build_dir_cache(struct vnode *dir_node) {
  Content *dir_content = content_ptr(dir_node);
  if (dir_content->node_type != FAT_NODE_TYPE_DIR) {
    return -1;
  }
  struct Dentry *dentry;
  int type;
  // Count available children (size of the dir)
  {
    uint32_t num_children = 0;
    dentry = (struct Dentry *)dir_content->data;
    for (int i = 0; i < dir_content->capacity; i++, dentry++) {
      type = dentry_type(dentry);
      if (type == FAT_DENTRY_UNUSED) {
        continue;
      } else if (type == FAT_DENTRY_END) {
        break;
      }
      num_children++;
    }
    dir_content->size = num_children;
  }

  // Build child vnodes
  {
    dir_content->children =
        (struct vnode **)kalloc(sizeof(struct vnode *) * dir_content->size);
    dentry = (struct Dentry *)dir_content->data;
    for (int num_built = 0; num_built < dir_content->size; dentry++) {
      type = dentry_type(dentry);
      if (type == FAT_DENTRY_UNUSED) {
        continue;
      }
      dir_content->children[num_built] = create_vnode_from_dentry(dentry);
      num_built++;
    }
  }
  dir_content->cached = true;
  return 0;
}

#ifdef CFG_RUN_FS_FAT_TEST
#define ENSURE_STRUCT_SIZE(_type, size)                                        \
  {                                                                            \
    int actual_size = sizeof(struct _type);                                    \
    if (actual_size != (size)) {                                               \
      uart_println("Test failed: size of %s, expect: %d, got: %d", #_type,     \
                   (size), actual_size);                                       \
      assert((sizeof(struct _type) == (size)));                                \
    }                                                                          \
  }

static bool test_fat_struct_size() {
  ENSURE_STRUCT_SIZE(Dentry, 32);
  ENSURE_STRUCT_SIZE(_Dentry_attr, 1);
  return true;
};
#endif
void test_fat() {
#ifdef CFG_RUN_FS_FAT_TEST
  unittest(test_fat_struct_size, "FS", "FAT - size of structs");
#endif
}