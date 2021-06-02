#include "fs/fat.h"
#include "fs/vfs.h"

#include "dev/mbr.h"
#include "dev/sd.h"

#include "mm.h"
#include "stdint.h"
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

// lazy init
int fat_initialized = false;
struct vnode_operations *fat_v_ops = NULL;
struct file_operations *fat_f_ops = NULL;

struct filesystem fat = {
    .name = "fat32", .setup_mount = fat_setup_mount, .next = NULL};

// Start address (block idx) of the FAT file system in the SD card
uint32_t fat_entry_block_idx;

struct FAT32_VolumnIDMeta {
  uint16_t BytsPerSec; // value should always be 512
  uint8_t SecPerClus;  // Sector per cluster
  uint16_t RsvdSecCnt;
  uint8_t NumFATs;    // always = 2
  uint32_t SecPerFAT; //
  uint32_t RootClus;  // Usually 0x00000002
};

// Parse FAT layout information from the
//  first sector of the filesystem(VolumnID)
static int parseFAT32_VolumnID(uint32_t lba_begin,
                               struct FAT32_VolumnIDMeta *target);

static void parseFATInfo();

#define FATAL(msg)                                                             \
  uart_println(msg);                                                           \
  while (1) {                                                                  \
    ;                                                                          \
  }

int parseFAT32_VolumnID(uint32_t lba_begin, struct FAT32_VolumnIDMeta *target) {
  int ret_val = 0;
  uint8_t *bfr = (uint8_t *)kalloc(512);
  readblock(lba_begin, bfr);

  if (bfr[510] != 0x55 || bfr[511] != 0xAA) {
    ret_val = 1;
    goto _fat_vol_parse_end;
  }
  uintptr_t sec_start = (uintptr_t)bfr;

  uint16_t signature = *(uint32_t *)(sec_start + 0x1FE);
  if (signature != 0xAA55) {
    FATAL("Corrupted FAT VolumnID");
  }
  target->BytsPerSec = *(uint16_t *)(sec_start + 0x0B);
  target->SecPerClus = *(uint8_t *)(sec_start + 0x0D);
  target->RsvdSecCnt = *(uint16_t *)(sec_start + 0x0E);
  target->NumFATs = *(uint8_t *)(sec_start + 0x10);
  target->SecPerFAT = *(uint32_t *)(sec_start + 0x24);
  target->RootClus = *(uint32_t *)(sec_start + 0x2C);
  ret_val = 0;

  if (target->NumFATs != 2 || target->BytsPerSec != 512) {
    uart_println("[FAT] numFAT:%d , bytePerSec:%d", target->NumFATs,
                 target->BytsPerSec);
    FATAL("FAT format not supported");
  }
_fat_vol_parse_end:
  kfree(bfr);
  return ret_val;
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

void fat_dev() { parseFATInfo(); }

void parseFATInfo() {
  unsigned char *buf = (unsigned char *)kalloc(512);
  readblock(0, buf);
  // The first block of the FAT filesystem is mbr
  struct MBR *mbr = (struct MBR *)buf;
  int num_parti;

  if (!mbr_is_valid(mbr)) {
    FATAL("Read invalid MBR");
  } else if (0 >= (num_parti = mbr_num_partitions(mbr))) {
    FATAL("No valid partition exists");
  }

  fat_entry_block_idx = mbr->partition_entry[0].lba_begin;
  log_println("[FAT] lba idx: %d", fat_entry_block_idx);

  struct FAT32_VolumnIDMeta meta;
  int fail;
  if (1 == (fail = parseFAT32_VolumnID(fat_entry_block_idx, &meta))) {
    uart_println("parse failed");
    goto _fat_parse_end;
  }
  uart_println("sector data:");
  uart_println("BytsPerSec: %d", meta.BytsPerSec);
  uart_println("SecPerClus: %d", meta.SecPerClus);
  uart_println("RsvdSecCnt: %d", meta.RsvdSecCnt);
  uart_println("NumFATs: %d", meta.NumFATs);
  uart_println("SecPerFAT: %d", meta.SecPerFAT);
  uart_println("RootClus: %d", meta.RootClus);

_fat_parse_end:
  kfree(buf);
}

int fat_init() {
  fat_v_ops = kalloc(sizeof(struct vnode_operations));
  fat_v_ops->lookup = fat_lookup;
  fat_v_ops->create = fat_create;

  fat_f_ops = kalloc(sizeof(struct file_operations));
  fat_f_ops->read = fat_read;
  fat_f_ops->write = fat_write;
  uart_println("fat inited");
  return 0;
}

int fat_setup_mount(struct filesystem *fs, struct mount *mount) {
  if (fat_initialized == false) {
    fat_init();
    fat_initialized = true;
  }
  uart_println("fat setup");
  return 0;
}

#ifdef CFG_RUN_FS_FAT_TEST
#endif
void test_fat() {
#ifdef CFG_RUN_FS_FAT_TEST
#endif
}