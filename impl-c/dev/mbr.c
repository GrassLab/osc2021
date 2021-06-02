#include "dev/mbr.h"

#include "dev/sd.h"
#include "mm.h"
#include "stdint.h"
#include "uart.h"

#include "log.h"
#include "test.h"

#ifdef CFG_LOG_DEV_MBR
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

#define BF_INACT 0x00
#define BF_VALID 0x80

bool mbr_is_valid(struct MBR *mbr) {
  return mbr->sanity_1 == 0x55 && mbr->sanity_2 == 0xAA;
}

int mbr_num_partitions(struct MBR *mbr) {
  int num_valid = 0;
  struct MBR_Entry *entry;
  for (int i = 0; i < 4; i++, num_valid++) {
    entry = &mbr->partition_entry[i];
    if (entry->boot_flag == BF_VALID && entry->lba_begin > 0) {
      if (i == 0) {
        log_println("Parse MBR parition table:");
        log_println(" ----");
      }
      log_println(" Entry %d: lba_begin:%d, num_sectors:%d", i,
                  entry->lba_begin, entry->num_sectors);
    } else {
      break;
    }
  }
  return num_valid;
}

#ifdef CFG_RUN_DEV_MBR_TEST
#define ENSURE_STRUCT_SIZE(_type, size)                                        \
  {                                                                            \
    int actual_size = sizeof(struct _type);                                    \
    if (actual_size != (size)) {                                               \
      uart_println("Test failed: size of %s, expect: %d, got: %d", #_type,     \
                   (size), actual_size);                                       \
      assert((sizeof(struct _type) == (size)));                                \
    }                                                                          \
  }

bool test_struct_size() {
  ENSURE_STRUCT_SIZE(MBR_Entry, 16);
  ENSURE_STRUCT_SIZE(MBR, 512);
  return true;
};
#endif

void test_mbr() {
#ifdef CFG_RUN_DEV_MBR_TEST
  unittest(test_struct_size, "DEV", "MBR - size of structs");
#endif
}
