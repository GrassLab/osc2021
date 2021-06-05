#pragma once

#include "bool.h"
#include "stdint.h"

// Parser for Master Boot Record (MBR) partition table

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

// Raw format of the Master Boot Record(MBR)
// 512 Bytes
struct MBR {
  uint8_t boot_code[446];              // not used, 446B
  struct MBR_Entry partition_entry[4]; // 64 Bytes
  uint8_t sanity_1;                    // 1B -> always 0x55
  uint8_t sanity_2;                    // 1B -> always 0xAA
} __attribute__((packed));

bool mbr_is_valid(struct MBR *mbr);

// Parse partition information stored in MBR
int mbr_num_partitions(struct MBR *mbr);

void test_mbr();
