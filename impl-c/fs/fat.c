#include "fs/fat.h"
#include "fs/vfs.h"

#include "dev/sd.h"
#include "mm.h"
#include "stdint.h"
#include "uart.h"
#include <stddef.h>

// lazy init
int fat_initialized = false;
struct vnode_operations *fat_v_ops = NULL;
struct file_operations *fat_f_ops = NULL;

struct filesystem fat = {
    .name = "fat32", .setup_mount = fat_setup_mount, .next = NULL};

// Partition entry
// 16 Bytes
struct MBR_Entry {
  uint8_t boot_flag;       // not used, 1B
  uint32_t chs_begin : 24; // not used, 3B
  uint8_t type_code;       // 1B
  uint32_t chs_end : 24;   // not used, 3B
  uint32_t lba_begin;      // begin address of the Logical Block), 4B
  uint32_t num_sectors;    // redundant data, 4B
} __attribute__((packed));

// Master Boot Record
// 512 Bytes
struct MBR {
  uint8_t boot_code[446];              // not used, 446B
  struct MBR_Entry partition_entry[4]; // 64 Bytes
  uint8_t sanity_1;                    // 1B -> always 0x55
  uint8_t sanity_2;                    // 1B -> always 0xAA
} __attribute__((packed));

int mbr_is_valid(struct MBR *mbr) {
  return mbr->sanity_1 == 0x55 && mbr->sanity_2 == 0xAA;
}

int mbr_get_num_valid_partitions(struct MBR *mbr) {
  int num_valid = 0;
  struct MBR_Entry *entry;
  for (int i = 0; i < 4; i++, num_valid++) {
    entry = &mbr->partition_entry[i];
    // Bootflag not in 0x00/0x80 is invalid. 0x00 means inactive
    if (entry->boot_flag == 0x80 && entry->lba_begin > 0) {
      uart_println("Entry%d: lba_begin:%d, size:%d", i, entry->lba_begin,
                   entry->num_sectors);
    } else {
      return num_valid;
    }
  }
}

void fat_dev() {
  unsigned char *buf = (unsigned char *)kalloc(512);
  readblock(0, buf);
  // The first block of the FAT filesystem is mbr
  struct MBR *mbr = (struct MBR *)buf;
  if (!mbr_is_valid(mbr)) {
    uart_println("Read invalid MBR");
  }
  int num_parti;
  if ((num_parti = mbr_get_num_valid_partitions(mbr)) < 1) {
    uart_println("No valid partition");
  }
  kfree(buf);
}

int fat_init() { return 0; }

int fat_setup_mount(struct filesystem *fs, struct mount *mount) {
  if (fat_initialized == false) {
    fat_init();
    fat_initialized = true;
  }
  uart_println("fat setup");
  return 0;
}